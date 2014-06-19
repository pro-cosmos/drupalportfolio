<?php
function subscription_mail_files($mail_id) {
  $headers = array(
    array('data' => t('Файл'), 'field' => 'filename', 'sort' => 'desc'),
    array('data' => t('Опции'), 'colspan' => 2, 'style' => 'text-align:center')
  );

  $sql = "select id, filename, filepath from {mailfile} where mid = $mail_id";
  $sql .= tablesort_sql($headers);
  $result = db_query($sql);
  $data = array();
  while ($resobject = db_fetch_object($result)) {
    $data[] = array(
      $resobject->filename,
      l(t('Upload'), file_create_url($resobject->filepath)),
      l(t('Delete'), variable_get('subscription_baseurl', '') . "/mail/file/".$resobject->id."/delete", array('attributes' => array('onclick' => 'return confirm(\'' . t('Вы действительно хотите удалить этот файл?') . '\');'))),
    );
  }

  $output = theme('table', $headers, $data);

  $output .= drupal_get_form('subscription_mail_file_form', $mail_id);
  return $output;
}

//function subscription_mail_file_delete($mail_id, $file_id) {
//  watchdog('filedel', $filepath);
//  $filepath = db_result(db_query("select filepath from {mailfile} where id = %d", $file_id));
//
//  file_delete($filepath);
//  db_query("delete from {mailfile} where id = %d", $file_id);
//  drupal_goto(variable_get('subscription_baseurl', '') . "/mail/$mail_id/files");
//}

function subscription_mail_file_form($form_state, $mail_id) {
  $form['w'] = array(
    '#type' => 'fieldset',
    '#title' => 'Загрузить файл',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['w']['mid'] = array(
    '#type' => 'hidden',
    '#value' => $mail_id
  );

  $form['w']['file'] = array(
    '#type' => 'file',
    '#title' => '',
  );
  $form['w']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Загрузить'
  );

  $form['#attributes']['enctype'] = 'multipart/form-data';


  return $form;
}

function subscription_mail_file_form_submit($form, &$form_state) {
  $mail_id = $form_state['values']['mid'];
  $dest = file_directory_path() . "/mails/$mail_id/";
  if (!file_check_directory($dest, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    mkdir($dest);
  }
  if (!is_dir($dest)) {
    drupal_set_message($dest . ' is not directory!');
  }
  else {
    if ($file = file_save_upload('file', array(), $dest)) {
      file_set_status($file, FILE_STATUS_PERMANENT);
      $record = new stdClass();
      $record->mid = $mail_id;
      $record->filename = $file->filename;
      $record->filepath = $file->filepath;
      $record->filemime = $file->filemime;
      drupal_write_record('mailfile', $record);
      drupal_set_message('Файл загружен');
    }
  }
}

function subscription_mail_add_edit_form($form_state, $id = '') {
  global $user;

  $form['mail'] = array(
    '#title' => 'Добавление письма',
    '#type' => 'fieldset',
  );

  $form['mail']['subject'] = array(
    '#title' => 'Тема',
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['mail']['mtext_tokens'] = array(
    '#title' => t('Tokens'),
    '#type' => 'fieldset',
    '#attributes' => array('style' => 'margin-bottom:0px'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  //get tokens provided by modules
  $tokens = module_invoke_all('subscription_message_tokens', $dummy_account);
  $rows = array();
  foreach ($tokens as $topic => $item) {
    if (!empty($rows)) {
      $rows[] = '';
    }

    $rows[] = '<b style="background:#F4F4F4;font-size:11px;padding:2px 3px;">' . t($topic) . '</b>';
    foreach ($item as $token => $value) {
      //common descr
      $token_descr = (t('tokeninfo ' . $token) == 'tokeninfo ' . $token) ? '' : t('tokeninfo ' . $token);
      //set subscription custom descr
      if ($topic == 'Subscriptions') {
        if (strpos($token, '|') === FALSE) {
          $token_descr = t('Subscribtion name');
        }
        if (!strpos($token, '|п') === FALSE) {
          $token_descr = t('Subscribe link');
        }
        if (!strpos($token, '|о') === FALSE) {
          $token_descr = t('Unsubscribe link');
        }
        if ($token == '|пв') {
          $token_descr = 'Subscribe to all subscriptions exists in mail message';
        }
      }
      $token_descr = !empty($token_descr) ? '- ' . $token_descr : '';

      $rows[] = '<span style="font-size:11px;color:#3399CC">{{' . $token . '}}</span> ' . $token_descr;
    }
  }

  $form['mail']['mtext_tokens']['tokens'] = array(
    '#type' => 'markup',
    '#value' => implode('<br>', $rows)
  );


  $form['mail']['mtext_texteditor']['mtext'] = array(
    '#title' => 'Текст',
    '#type' => 'textarea',
  );
  $form['mail']['mtext_texteditor']['format'] = filter_form(FILTER_HTML_STRIP);

  $mail_types = load_catalog('mail');
  $form['mail']['type'] = array(
    '#title' => 'Тип отправления',
    '#type' => 'select',
    '#options' => $mail_types,
    '#default_value' => 1
  );

  $form['mail']['delay'] = array(
    '#title' => 'Задержка',
    '#type' => 'textfield',
    '#default_value' => 0,
    '#element_validate' => array('subscription_validate_integer')
  );

  $evenytypes = load_catalog('event');
  $form['mail']['eventtype'] = array(
    '#title' => 'Тип события',
    '#type' => 'select',
    '#options' => $evenytypes,
    '#default_value' => 1,
  );

  $subscriptions = load_subscriptions();
  if (count($subscriptions) > 0) {
    $form['mail']['subscription'] = array(
      '#title' => 'Рассылка',
      '#type' => 'select',
      '#options' => $subscriptions,
    );
  }
  else {
    unset($mail_types[4]);
    $form['mail']['type']['#options'] = $mail_types;
  }

  $form['mail']['filters'] = array(
    '#title' => 'Фильтры получателей',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $countries = Country::select_list();
  $countries[-1] = t('All');
  $form['mail']['filters']['country'] = array(
    '#title' => 'Страна',
    '#type' => 'select',
    '#options' => $countries,
    '#default_value' => -1,
  );

  $form['mail']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Добавить письмо',
    '#weight' => 100
  );

  if (is_numeric($id) && $id > 0) {
    $data = Mail::get($id);
    if ($data) {
      $form['mail']['id'] = array('#type' => 'value', '#value' => $id);
      $form['mail']['subject']['#default_value'] = $data->subject;
      $form['mail']['mtext_texteditor']['mtext']['#default_value'] = $data->mtext;
      $form['mail']['type']['#default_value'] = $data->stype;
      $form['mail']['delay']['#default_value'] = $data->delay;

      if ($data->stype == SUBSCRIPTION_MAIL_TYPE_EVENT) {
        $eventtype = db_fetch_object(db_query("select eid from {mailevent} where mid=$id"));
        $form['mail']['eventtype']['#default_value'] = $eventtype->eid;
      }
      if ($data->stype == SUBSCRIPTION_MAIL_TYPE_SUBSCRIPTION) {
        $subscription = db_fetch_object(db_query("select sid from {mailsubscription} where mid=$id"));
        $form['mail']['subscription']['#default_value'] = $subscription->sid;
      }

      //filters
      $country = get_filter_value('user country', $id);
      $form['mail']['submit']['#value'] = 'Сохранить изменения';
      $form['mail']['filters']['country']['#default_value'] = $country;
    }
  }


  //Hide Delay for Event type mails
  drupal_add_js("
     $(document).ready(function () {
        $('#edit-type').change(function () {
          if ($('#edit-type').val() != 3)
            $('#edit-eventtype-wrapper').hide('slow');
          else
            $('#edit-eventtype-wrapper').show('slow');
          if ($('#edit-type').val() != 2)
            $('#edit-subscription-wrapper').hide('slow');
          else
            $('#edit-subscription-wrapper').show('slow');
        });
        if ($('#edit-type').val() != 3){
          $('#edit-eventtype-wrapper').hide();
        };
        if ($('#edit-type').val() != 2){
          $('#edit-subscription-wrapper').hide();
        };
     });", "inline");

  return $form;
}

function subscription_validate_integer($element, $form_state) {
  if (!is_numeric($form_state['values']['delay'])) {
    form_error($element, t("Поле Задержка должно быть целым цислом."));
  }
  if ($form_state['values']['delay'] < 0) {
    form_error($element, t("Поле Задержка должно быть больше или равно 0."));
  }
}

function subscription_mail_add_edit_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];

  $record = new stdClass();
  $record->subject = $form_values['subject'];
  $record->mtext = $form_values['mtext'];
  $record->attachments = '';
  $record->delay = $form_values['delay'];
  $record->created = time();
  $record->stype = $form_values['type'];

  $create = TRUE;
  $old_mail = NULL;
  if ($form_values['id'] != NULL) {
    $create = FALSE;
    $mail_id = $form_values['id'];
    $old_mail = DB::fetch_object("select id, stype from {mail} where id = " . (int) $mail_id);
  }
  if ($create) {
    db_query("update {mail} set weight = weight + 1");
    $record->weight = 0;
    drupal_write_record('mail', $record);
    drupal_set_message(t('Письмо добавлено.'));
  }
  else {
    $record->id = $form_values['id'];
    drupal_write_record('mail', $record, 'id');
    drupal_set_message(t('Изменения сохранены.'));
  }
  remove_all_filters($record->id);
  save_filter('user country', $record->id, $form_values['country']);

  if ($old_mail != NULL) { //old mail, change type if needed
    $mail_id = $old_mail->id;
    if ($old_mail->stype != $record->stype) {
      remove_mail_from_queue($mail_id);
      remove_all_links_to_events_and_subscriptions($mail_id);
      if ($record->stype == SUBSCRIPTION_MAIL_TYPE_EVENT) {
        add_link_to_event($mail_id, $form_values['eventtype']);
      }
      if ($record->stype == SUBSCRIPTION_MAIL_TYPE_SUBSCRIPTION) {
        add_link_to_subscription($mail_id, $form_values['subscription'], $record->delay);
      }
    }
    else {
      if ($old_mail->stype == SUBSCRIPTION_MAIL_TYPE_EVENT) {
        $event = DB::fetch_result("select eid from {mailevent} where mid = $mail_id");

        if ($event != $form_values['eventtype']) {
          remove_mail_from_queue($mail_id);
          $eventtype = $form_values['eventtype'];
          add_link_to_event($mail_id, $form_values['eventtype']);
        }

      }
      else {
        if ($old_mail->stype == 4) {
          $sub = DB::fetch_result("select sid from {mailsubscription} where mid = $mail_id");
          if ($sub != $form_values['subscription']) {
            remove_mail_from_queue($mail_id);
            $sub_id = $form_values['subscription'];
            db_query("update {mailsubscription} set sid = $sub_id where mid = $mail_id");
            subscription_on_mail_added_to_subscription($mail_id, $sub_id, $record->delay);
          }
        }
      }
    }
  }
  else { //new mail, create link to event or subscription
    if ($record->stype == SUBSCRIPTION_MAIL_TYPE_EVENT) {
      add_link_to_event($record->id, $form_values['eventtype']);
    }
    if ($record->stype == SUBSCRIPTION_MAIL_TYPE_SUBSCRIPTION) {
      add_link_to_subscription($record->id, $form_values['subscription'], $record->delay);
    }
  }

  preg_match_all(_wysiwyg_imageupload_filter_pattern(), $form_values['mtext'], $matches, PREG_PATTERN_ORDER);
  if ($matches[1]) {
    $iids = array();
    foreach ($matches[1] as $iid) {
      $iids[] = $iid;
    }

    db_query("
      UPDATE `files` f
      LEFT JOIN `wysiwyg_imageupload_entity` wue ON wue.fid = f.fid
      SET f.status = 1
      WHERE f.status = 0 AND wue.iid IN (%s)", implode($iids, ', ')
    );
  }

  $form_state['redirect'] = variable_get('subscription_baseurl', '') . '/mail';
}

function add_link_to_event($id, $event) {
  $record = new stdClass();
  $record->mid = $id;
  $record->eid = $event;
  $ins = drupal_write_record('mailevent', $record);

  if (!$ins) {
    db_query("update {mailevent} set eid = %d where mid = %d", $event, $id);
  }
}

function add_link_to_subscription($id, $sub, $delay) {
  $subscription_record = new stdClass();
  $subscription_record->mid = $id;
  $subscription_record->sid = $sub;
  drupal_write_record('mailsubscription', $subscription_record);
  subscription_on_mail_added_to_subscription($id, $sub, $delay);
}

function remove_mail_from_queue($id, $delete_sent = FALSE) {
  $query = "delete from {usermail} where mid = %d";
  if (!$delete_sent) {
    $query .= " and status = 0";
  }
  db_query($query, $id);
}

/*
 * Get all users who has this subscription and status is active
 * add this message to all subscribers
 */
function subscription_on_mail_added_to_subscription($mail_id, $sub_id, $delay) {
  $country_filter = get_filter_value('user country', $mail_id);
  $subscriber_sql = "select s.uid, s.atime from {substatus} s
  inner join {ori_subscription_user} u on s.uid = u.id
  where s.sid = $sub_id and s.stid = 2 and u.mail_status=1 ";
  if ($country_filter != -1) {
    $subscriber_sql .= " and s.cid = $country_filter";
  }
  $subscribers_result = db_query($subscriber_sql);
  while ($subscriber = db_fetch_object($subscribers_result)) {
    $record = new stdClass();
    $record->uid = $subscriber->uid;
    $record->mid = $mail_id;
    $record->status = 0;
    $delay = subscription_convert_delay($delay);
    $record->senddate = subscription_today() > $subscriber->atime + $delay ?
      subscription_today() :
      $subscriber->atime + $delay;
    // Send tomorrow if old subscriber or send with delay from activation time if new
    drupal_write_record('usermail', $record);
  }
}

function remove_all_links_to_events_and_subscriptions($id) {
  DB::delete('mailevent', 'mid', $id);
  DB::delete('mailsubscription', 'mid', $id);
}

function remove_all_filters($id) {
  DB::delete('mailfiltervalue', 'mid', $id);
}

function save_filter($filter, $id, $value) {
  $record = new stdClass();
  $record->filter = $filter;
  $record->mid = $id;
  $record->value = $value;
  drupal_write_record('mailfiltervalue', $record);
}


function subscription_mail_list() {

  //$output .= drupal_get_form('pms_users_filter_form');
  $output .= drupal_get_form('subscription_mail_list_form');

  return $output;
}


/**
 * show selected list
 */
function _subscription_mail_list_selected() {
  $out = '';

  if (_subscription_mail_list_is_selected()) {
    drupal_add_js(drupal_get_path('module', 'pms_users') . '/css/pms_users.list.css');
    $listyle = 'style="margin-left:10px;list-style: square outside !important;"';
    $ulstyle = 'style="margin:10px 0px;list-style: square outside !important;"';

    if (is_array($_SESSION['subscription_mail_list'])) {
      if (count($_SESSION['subscription_mail_list']) < 50) {
        $selecteditems = array();
        //query
        $sql = "select m.subject as title from {mail} m where m.id in (" . implode(',', $_SESSION['subscription_mail_list']) . ")";

        $q = db_query($sql);
        while ($row = db_fetch_array($q)) {
          $selecteditems[] = $row['title'];
        }
        $out = '<ul class="selected-items" ' . $ulstyle . '><li ' . $listyle . ' >' . implode('</li><li ' . $listyle . '>', $selecteditems) . '</li></ul>';
      }
      else {
        $out = count($_SESSION['subscription_mail_list']) . ' ' . t('items selected');
      }
    }

    if ($_SESSION['subscription_mail_list'] == 'all') {
      $out = t('All');
    }

  }

  return $out;
}


/**
 * User list form
 * @return string
 */
function subscription_mail_list_form($form_state) {

  $form = array();

  _subscription_mail_list_form_top($form);

  // table
  $form['table']['#value'] = _subscription_mail_list_table();

  //bottom buttons
  _subscription_mail_list_form_bottom($form);


  $form['#submit'][] = 'subscription_mail_list_form_submit';

  return $form;
}

function _subscription_mail_list_is_selected() {
  return !empty($_SESSION['subscription_mail_list']);
}

function _subscription_mail_list_form_top(&$form) {
  $selected = _subscription_mail_list_is_selected();
  if ($selected) {
    $showselected = is_array($_SESSION['subscription_mail_list']) ? count($_SESSION['subscription_mail_list']) : t($_SESSION['subscription_mail_list']);
  }


//top buttons
  $form['buttons_top_operations'] = array(
    '#type' => 'fieldset',
    '#title' => t('Group operations'),
  );

  //if(user_access())
  $form['buttons_top_operations']['saveorder'] = array(
    '#type' => 'submit',
    '#value' => t('Save order')
  );
//  $form['buttons_top_operations']['saveutf'] = array(
//    '#type' => 'submit',
//    '#value' => t('Save selected mails as UTF-8'),
//    '#submit' => array('subscription_mail_utf'),
//  );
  if ($selected) {
    $form['buttons_top_operations']['goselectusers'] = array(
      '#type' => 'submit',
      '#value' => t('Select mails recipients')
    );
  }


//top buttons
  $form['buttons_top'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select options') . ($selected ? ' (' . $showselected . ')' : ''),
  );

  //add selected list
  if ($selected) {
    $form['buttons_top']['selected']['#value'] = _subscription_mail_list_selected();
    $form['buttons_top']['selected']['#prefix'] = t('Selected items') . ':<br /><div class="group_operations_description" >';
    $form['buttons_top']['selected']['#suffix'] = '</div>';
  }


  //select button if(user_access())
  if ($_SESSION['subscription_mail_list'] != 'all') {
    $form['buttons_top']['select'] = array(
      '#type' => 'submit',
      '#value' => t('Add selected')
    );
  }

  if (!$selected) {
    $form['buttons_top']['selectall'] = array(
      '#type' => 'submit',
      '#value' => t('Select All')
    );
  }

  //reset button
  $form['buttons_top']['resetselect'] = array(
    '#type' => 'submit',
    '#value' => t('Reset')
  );
}


function _subscription_mail_list_form_bottom(&$form) {
  $selected = _subscription_mail_list_is_selected();


  $form['buttons_bottom'] = array(
    '#type' => 'fieldset',
    '#title' => t('Buttons'),
    '#attributes' => array('style' => 'margin:10px 0px')
  );

  $form['buttons_bottom']['pagesize'] = array(
    '#type' => 'select',
    '#options' => drupal_map_assoc(array(10, 20, 50, 100, 300)),
    '#default_value' => (empty($_SESSION['subscription_mail_filters']['pagesize']) ? 10 : (int) $_SESSION['subscription_mail_filters']['pagesize']),
    '#prefix' => '<div id="pagesize">Page size',
    '#suffix' => '</div>',

  );


}


function subscription_mail_list_form_submit($form, &$form_state) {

  $ids = isset($form['#post']['id']) ? $form['#post']['id'] : FALSE;
  $butt = isset($form['#post']['op']) ? $form_state['clicked_button']['#id'] : '';


  //$selected_is_array = isset($_SESSION['subscription_mail_list']) && !in_array($_SESSION['subscription_mail_list'], array('all','filtered'));
  if (in_array($butt, array('edit-select', 'edit-goselectusers')) && is_array($ids)) {
    //save selected
    $_SESSION['subscription_mail_list'] = empty($_SESSION['subscription_mail_list']) ? array() : $_SESSION['subscription_mail_list'];
    if (is_array($_SESSION['subscription_mail_list'])) {
      $_SESSION['subscription_mail_list'] = array_unique(array_merge($_SESSION['subscription_mail_list'], array_keys($ids)));
    }
  }

  if ($butt == 'edit-goselectusers') {
    //redirect to user list
    //after the admin select mails to send
    drupal_goto(variable_get('subscription_userlist_page', '') . '/groupaction/mailsid_send');
  }

  //empty selected items
  if ($butt == 'edit-resetselect') {
    //save selected
    unset($_SESSION['subscription_mail_list']);
  }

  // select all
  if ($butt == 'edit-selectall') {
    $_SESSION['subscription_mail_list'] = 'all';
  }

  // save order of mails
  if ($butt == 'edit-saveorder') {
    subscription_mail_changeorder();
  }


  //set current page size
  if ($form_state['values']['pagesize'] > 0) {
    $_SESSION['subscription_mail_filters']['pagesize'] = $form_state['values']['pagesize'];
  }

}

function subscription_mail_utf($form, &$form_state){

  $ids_mail = $_SESSION['subscription_mail_list'];
  $mails = db_query("SELECT * FROM {mail} WHERE id IN (" . db_placeholders($ids_mail) . ")", $ids_mail);
  while ($mail = db_fetch_object($mails)) {
    $text = mb_convert_encoding ($mail->mtext, "UTF-8", mb_detect_encoding($mail->mtext));
    db_query("UPDATE {mail} SET mtext = '%s' WHERE id = %d", $text, $mail->id);

  }

}

function _subscription_mail_list_query_where() {

  $sql = "select m.weight, m.id, m.subject, m.delay, s.title as sub, t.value as mtype, e.value as event from {mail} m " .
    "left join {catalog} t on t.type='mail' and t.id = m.stype " .
    "left join {subscription} s on s.id = (select ms.sid from {mailsubscription} ms where ms.mid = m.id) " .
    "left join {catalog} e on e.type='event' and e.id = (select me.eid from {mailevent} me where me.mid = m.id)  ";

  $sql_count = "select count(m.id) from {mail} m " .
    "left join {catalog} t on t.type='mail' and t.id = m.stype " .
    "left join {subscription} s on s.id = (select ms.sid from {mailsubscription} ms where ms.mid = m.id) " .
    "left join {catalog} e on e.type='event' and e.id = (select me.eid from {mailevent} me where me.mid = m.id)  ";


  return array('sql' => $sql, 'sql_count' => $sql_count);
}

/**
 *
 * @return string
 */
function _subscription_mail_list_table() {

  drupal_add_js(drupal_get_path('module', 'pms_users') . '/js/pms_users.list.js');

  $selall_checkbox = array(
    '#type' => 'checkbox',
    '#id' => 'selAllUsers',
    '#name' => 'selAll',
    '#default_value' => ''
  );

  $headers = array(
    array('data' => '#', 'field' => 'm.weight'),
    array('data' => drupal_render($selall_checkbox)),
    array('data' => 'Тема'),
    array('data' => 'Тип'),
    array('data' => 'Задержка'),
    array('data' => 'Рассылка'),
    array('data' => 'Событие'),
    array('data' => t('Actions'), 'colspan' => 2, 'style' => 'text-align:center')
  );

  // build query
  $qresult = _subscription_mail_list_query_where();
  extract($qresult); //return sql sql_count
  //default sort
  if (empty($_GET['order'])) {
    $_GET['order'] = '#';
    $_GET['sort'] = 'asc';
  }
  $sql .= tablesort_sql($headers);

  $limit = empty($_SESSION['subscription_mail_filters']['pagesize']) ? 10 : (int) $_SESSION['subscription_mail_filters']['pagesize'];
  $result = pager_query($sql, $limit, 0, $sql_count);
  global $pager_total_items;
  $count = $pager_total_items[0];


  $rows = array();
  $selected = _subscription_mail_list_is_selected();
  if ($selected) {
    $selectedids = $_SESSION['subscription_mail_list'];
  }


  while ($row = db_fetch_object($result)) {

    $num_checkbox = array(
      '#type' => 'checkbox',
      '#id' => 'id' . $row->id,
      '#name' => 'id[' . $row->id . ']',
      '#attributes' => array('class' => 'idCheckbox'),
      '#default_value' => 1
    );
    //check selected checkbox
    if ($selected) {
      $checked = FALSE;
      if (is_array($selectedids) && in_array($row->id, $selectedids)) {
        $checked = 'checked';
      }
      if ($selectedids == 'all') {
        $checked = 'checked';
      }

      if ($checked) {
        $num_checkbox['#attributes']['checked'] = 'checked';
      }
    }

    $files = array();
    $sql = "select filename from {mailfile} where mid = $row->id";
    foreach (DB::fetch_array($sql) as $file) {
      $files[] = $file->filename;
    }
    $files = implode('<br/>', $files);
    $rows[] = array(
      'id' => "row-$row->id",
      'data' => array(
        array('class' => 'weight-holder', 'data' => "<span type=\"text\" class=\"weight-holder\" />"),
        drupal_render($num_checkbox),
        l($row->subject, variable_get('subscription_baseurl', '') . "/mail/$row->id/edit"),
        $row->mtype,
        $row->delay,
        $row->sub,
        $row->event,
        l('Файлы', variable_get('subscription_baseurl', '') . "/mail/$row->id/files"), // . "<span>$files</span>",
        l('Удалить', variable_get('subscription_baseurl', '') . "/mail/$row->id/delete", array('attributes' => array('onclick' => 'return confirm(\'' . t('Вы действительно хотите удалить это письмо?') . '\');'))),
      )
    );
  }


  $out = theme('table', $headers, $rows, '', 'Found rows: ' . $count);
  $out .= theme('pager', NULL, $limit, 0);


  drupal_add_js(drupal_get_path('module', 'subscription') . "/inc/subscription_mail/subscription.mail.list.js");
  drupal_add_js(drupal_get_path('module', 'subscription') . "/inc/subscription_mail/jquery.tablednd_0_5.js");
  return $out;
}

function subscription_mail_changeorder() {
  //get page number $pagenum
  $url = parse_url($_GET['page']);
  $pagenum = 1;
  if (isset($url['query'])) {
    parse_str($url['query']);
    if (isset($page)) {
      $pagenum = $page + 1;
    }
  }


  $order = $_GET['order'];
  $order = explode('&', $order);
  $new_order = array();

  $i = 0;
  $add = 10000; //max  items on 1 page
  foreach ($order as $o) {
    //get item id
    $id = explode('=', $o);
    if (!isset($id[1])) {
      continue;
    }
    $id = $id[1];
    if ($id == NULL or $id == '') {
      continue;
    }

    //calculate new weight
    $weight = ($pagenum * $add) + $i;

    db_query("update {mail} set weight = %d where id = %d", $weight, $id);

    $i++;
  }
  print drupal_to_js(array('status' => 'ok'));
}


function subscription_generate_selected_users_block() {
//  $_SESSION['selected_users'] = array();//do not forget to remove
  $selected_users = $_SESSION['selected_users'];
  if ($selected_users == NULL) {
    $selected_users = array();
  }

  $output = "<div id='selected-users-div'>";
  foreach ($selected_users as $key => $user) {
    if ($user == NULL) {
      continue;
    }
    $output .= _generate_span_for_user($user);
  }
  $output .= "</div>";
  return $output;
}

function _generate_span_for_user($user) {
  $resultUser = User::get($user);
  $name = $resultUser->lastname . ' ' . $resultUser->firstname;
  return "<span class='user-name' id='" . $user . "-span'>&lt; $name <a href='/' id='" . $user . "' onclick='return deleteUser(this);'>[x]</a> &gt; </span>";
}

function subscription_add_new_user_to_selected() {
  $selected_users = $_SESSION['selected_users'];
  if ($selected_users == NULL) {
    $selected_users = array();
  }
  $ids = $_GET['ids'];
  $debug = $ids;
  $output = "";
  $ids = split('-', $ids);
  sort($ids);
  foreach ($ids as $id) {
    $debug .= ',' . $id;
    if ($id == '') {
      continue;
    }
    if ($selected_users[$id] == $id) {
      continue;
    }
    $selected_users[$id] = $id;
    $output .= _generate_span_for_user($id);
  }
  $_SESSION['selected_users'] = $selected_users;
  print drupal_to_js(array('result' => 'ok', 'data' => $output, 'debug' => $debug));
}

function subscription_mail_delete($id) {
  if (is_numeric($id) && $id > 0) {
    db_query("delete from {mail} where id = $id");
    remove_all_links_to_events_and_subscriptions($id);
    remove_mail_from_queue($id, TRUE);

    drupal_goto(variable_get('subscription_baseurl', '') . '/mail');
  }
}

function subscription_remove_user_from_selected() {
  $selected_users = $_SESSION['selected_users'];
  $id = $_GET['id'];
  $debug = $id;
  unset($selected_users[$id]);
  $_SESSION['selected_users'] = $selected_users;
  print drupal_to_js(array('result' => 'ok', 'debug' => $debug));
}


/**
 *
 * @param type $selectedUsers
 * @return type
 */
function subscription_mail_send_users($selectedUsers, $selectedMails) {
  //check selected users
  if (empty($selectedUsers)) {
    return t('You must select users before!');
  }
  //check selected mails
  if (empty($selectedMails)) {
    return t('You must select mails !url before!', array('!url' => l('here', variable_get('subscription_baseurl', '') . '/mail')));
  }


  if (is_array($selectedUsers) || in_array($selectedUsers, array('filtered', 'all'))) {
    //add to queue
    foreach ($selectedMails as $k => $mail) {
      $sql_users = _subscription_get_selected_users_query($selectedUsers, $mail->id);

      $ok = db_query("INSERT IGNORE INTO {usermail} (id,uid,mid,senddate,status) (" . $sql_users . ")");
      $affected = db_affected_rows();

      watchdog('subscription', 'Cron: Mail "%mail" added to send queue for %num users', array(
        '%mail' => $mail->subject,
        '%num' => $affected
      ), WATCHDOG_INFO);
      drupal_set_message(t('There are @num messages added to send queue', array('@num' => $affected)));
    }

    if ($ok) {
      if (count($selectedMails) == 1) {
        return _subscription_get_sendmails_message($selectedMails);
      }
      else {
        return t('Message sucessfully added to queue');
      }
    }

  }

  return t('Cant add mails to send queue!');
}

/**
 *
 * @param type $selected
 * @return type
 */
function _subscription_get_selected_users_query($selected, $mid) {
// make sql query
  $sql = '';
  $senddate = strtotime(date('d-m-Y') . " UTC"); //allow send 1 unique message per day
  $status = 0;
  $fieldList = implode(',', array("''", "u.id", $mid, $senddate, $status));
  $filters = $_SESSION['pms_users_filters'];
  //dpm($filters);
  switch ($selected) {

    case 'all':
      $sql = "select " . $fieldList . " from {ori_user} u ";
      $sql .= " inner join {ori_subscription_user} su on su.id = u.id";
      $sql .= " where su.mail_status = 1 and u.status != ". PMS_USERS_STATUS_DISABLED;
      break;

    case 'filtered':
      module_load_include('inc', 'pms_users', 'pms_users.pages');
      $addfilters = array('mail_status' => 1); //, 'status' => PMS_USERS_STATUS_DISABLED);
      $sql_filtered = _pms_users_list_query_where($addfilters);
      $sql_filtered = $sql_filtered['sql'];
      $tmp = explode(' from ', strtolower($sql_filtered), 2);
      $sql = "select " . $fieldList . " from " . $tmp[1];

      break;

    default:
      //custom select
      if (is_array($selected)) {
        $sql = "select " . $fieldList . " from {ori_user} u ";
        $sql .= " inner join {ori_subscription_user} su on su.id = u.id";
        $sql .= " where su.mail_status = 1 AND u.status != " . PMS_USERS_STATUS_DISABLED;
        $sql .= " and u.id in (" . implode(',', $selected) . ")";
      }
  }

  return $sql;
}

function _subscription_get_sendmails_message($mails) {
  $subjects = "";
  foreach ($mails as $mail) {
    $subjects .= "<span style='color:blue;'>'$mail->subject'</span><span> </span><br>";
  }
  return "Сообщение(я) в процессе отправки: <br />" . $subjects;
}

function subscription_remove_all_selected_users() {
  unset($_SESSION['selected_users']);
}

function subscription_get_mails() {
  $mails = array();

  $selected = $_SESSION['subscription_mail_list'];

  if (!empty($selected) && (is_array($selected) || $selected = 'all')) {
    $sql = "select id, subject, mtext, delay from {mail} ";
    if (is_array($selected)) {
      $sql .= "where id in (" . implode(',', $selected) . ")";
    }


    foreach (DB::fetch_array($sql) as $resmail) {
      $mail = new stdClass();
      $mail->id = $resmail->id;
      $mail->subject = $resmail->subject;
      $mail->text = subscription_prepare_page_mail_text($resmail->mtext);
      $mails[] = $mail;
    }
  }

  return $mails;
}

function subscription_mail_resend($mail_id, $account) {
  if ($mail_id > 0 && $account && (pms_users_admin_access() || user_access('administer subscription mails'))) {
     subscription_send_mail_to_user($mail_id, $account);
  }
  //go to prev page
  drupal_goto(drupal_get_destination());
}

function subscription_mail_queue_delete($id) {
  if ($id > 0 && (pms_users_admin_access() || user_access('administer subscription mails'))) {
     _subscription_mail_queue_delete($id);
  }
  //go to prev page
  drupal_goto(drupal_get_destination());
}


function _subscription_mail_queue_delete($id) {
  if ($id>0) {
      db_query("delete from {usermail} where id = %d", $id);
  }
}