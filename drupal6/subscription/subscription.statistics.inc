<?php

function subscription_main_statistic(){
  $output = "";
  $output .= _generate_table_with_disabled_mail_users();
  $output .= _generate_table_with_subscription_statisitics();

  drupal_add_js(drupal_get_path('module', 'subscription').'/js/subscription.user.stat.js');
  return $output;
}

function _generate_table_with_disabled_mail_users(){
  $headers = array(
    array('data' => t('ФИО')),
    array('data' => t('Email')),
    array('data' => t('Дата отключения')),
  );

  $sql = "select u.id, u.firstname, u.lastname, u.middlename, u.email, su.mail_status_date
          from {ori_user} u inner join {ori_subscription_user} su on u.id= su.id where su.mail_status = 0";
  $sql .= tablesort_sql($headers);

  $result = db_query($sql);
  $data = array();
  $count=0;
  while($u = db_fetch_object($result)){
    $row = array();
    $row[] = "$u->lastname $u->firstname $u->middlename";
    $row[] = $u->email;
    $row[] = date('d.m.Y', $u->mail_status_date);
    $data[] = $row;
    $count++;
  }

  $table = theme('table', $headers, $data);
  if ($count > 0){
    $output =
          "<div><fieldset class='collapsible collapsed'>
            <legend class='collapse-processed'><a href='#' class='collapser'>Отключенные от писем: $count</a></legend>
            <div class='fieldset-wrapper' style='display: block;'>
              $table
            </div>
          </fieldset></div>";
    return $output;
  }
  return '';
}

function _generate_table_with_subscription_statisitics(){
  $output = "";

  $sub_result = db_query("select id, title from {subscription}");

  while ($sub = db_fetch_object($sub_result)){
    $headers = array(
      array('data' => t('ФИО')),
      array('data' => t('Email')),
      array('data' => t('Дата подключения')),
    );


    $sql = "select u.id, u.firstname, u.lastname, u.middlename, u.email, ss.atime from {ori_user} u inner join {substatus} ss on u.id = ss.uid and ss.sid = $sub->id and ss.stid = 2";
    $query_count = "select count(u.id) from {ori_user} u inner join {substatus} ss on u.id = ss.uid and ss.sid = $sub->id and ss.stid = 2";
    $sql .= tablesort_sql($headers);

    $result = db_query($sql);
    $data = array();
    while($resobject = db_fetch_object($result)){
      $row = array();
      $row[] = "$resobject->lastname $resobject->firstname $resobject->middlename";
      $row[] = $resobject->email;
      $row[] = date('d.m.Y', $resobject->atime);
      $data[] = $row;
    }

    $table = theme('table', $headers, $data);
    $count = db_result(db_query($query_count));
    if ($count > 0){
      $output .=
            "<div><fieldset class='collapsible collapsed'>
              <legend class='collapse-processed'><a href='#' class='collapser'>Подключены к рассылке '$sub->title': $count</a></legend>
              <div class='fieldset-wrapper' style='display: block;'>
                $table
              </div>
            </fieldset></div>";
    }
  }
  return $output;
}
