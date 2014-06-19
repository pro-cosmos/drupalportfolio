<?php

function subscription_subscription_list() {
  $headers = array(
    array('data' => t('Название'), 'field' => 'title', 'sort' => 'desc'),
    array('data' => t('Сокращение'), 'field' => 'shortname'),
    array('data' => t('Только новые'), 'field' => 'onlynew', 'style' => 'width:100px'),
    array('data' => t('Delete'),'style' => 'text-align:center; width:150px')
  );

  $sql = "select * from {subscription}";
  $query_count = "select count(id) from {subscription}";
  $sql .= tablesort_sql($headers);
  $limit = 15;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  while($row = db_fetch_object($result)){
    $data[] = array(
      l($row->title, variable_get('subscription_baseurl','')."/subscription/$row->id/edit"),
      $row->shortname,
      ($row->onlynew? 'Да':''),
      l(t('Delete'), variable_get('subscription_baseurl','')."/subscription/$row->id/delete", array('attributes' => array('onclick' => 'return confirm(\'Вы действительно хотите удалить эту рассылку?\');'))),
    );
  }

  $output = theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;
}

function subscription_subscription_add_edit_form($form_state, $id='') {

  $form['sub'] = array(
    '#title' => 'Добавление рассылки',
    '#type' => 'fieldset',
  );

  $form['sub']['stitle'] = array(
    '#title' => 'Название',
    '#type' => 'textfield',
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $form['sub']['shortname'] = array(
    '#title' => 'Сокращение',
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 30,
  //  '#description' => t('Введите сокращение для использования в пиьмах. <br/> {{сокращение}} - название рассылки<br/>  {{сокращение|п}} - ссылка для запроса на подписку к рассылке<br/>  {{сокращение|о}} - ссылка для отписки от рассылки'),
    '#required' => TRUE,
  );

  $form['sub']['smsg'] = array(
    '#title' => 'Текст ссылки на подписку к рассылке',
    '#type' => 'textfield',
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => false,
    '#default_value' => variable_get(SUBSCRIBE_TEXT, ''),
  );

  $form['sub']['umsg'] = array(
    '#title' => 'Текст ссылки на отписку от рассылке',
    '#type' => 'textfield',
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => false,
    '#default_value' => variable_get(UNSUBSCRIBE_TEXT, ''),
  );

  $form['sub']['additional'] = array(
    '#title' => 'Дополнительные параметры',
    '#type' => 'checkboxes',
    '#options' => array('only new' => 'Только новые'),
    //'#description' => t("'Только новые?' - Пользователь не получит письма, которые были добавлены до его активации в рассылке."),
  );

  $form['sub']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Добавить рассылку',
  );

  if (is_numeric($id) && $id > 0) {
    $data = Subscription::get($id);
    if ($data) {
      $form['sub']['id'] = array('#type' => 'value', '#value' => $id);
      $form['sub']['stitle']['#default_value'] = $data->title;
      $form['sub']['shortname']['#default_value'] = $data->shortname;
      $form['sub']['smsg']['#default_value'] = $data->smsg;
      $form['sub']['umsg']['#default_value'] = $data->umsg;
      $additional_data = array();
      if ($data->onlynew){
        $additional_data[] = 'only new';
      }
      $form['sub']['additional']['#default_value'] = $additional_data;
      $form['sub']['submit']['#value'] = t('Сохранить изменения');
    }
  }

  return $form;
}

function subscription_subscription_add_edit_form_validate($form, &$form_state) {
  if ($form_state['values']['id'] == NULL and db_fetch_object(db_query("select count(id) as value from {subscription} where shortname = '".$form_state['values']['shortname']."'"))->value > 0){
    form_error($form, "Рассылка с таким Сокращением уже существует.");
  }
}

function subscription_subscription_add_edit_form_submit($form, &$form_state) {
  $record = new stdClass();
  $record->title = $form_state['values']['stitle'];
  $record->smsg = $form_state['values']['smsg'];
  $record->umsg = $form_state['values']['umsg'];
  $record->shortname = $form_state['values']['shortname'];
  $record->onlynew = in_array('only new', array_filter($form_state['values']['additional']));
  $record->admincontrolled = in_array('admincontrolled', array_filter($form_state['values']['additional']));
  if ($form_state['values']['id'] == NULL) {
    drupal_write_record('subscription', $record);
  }
  else {
    $record->id = $form_state['values']['id'];
    drupal_write_record('subscription', $record, 'id');
  }

  $form_state['redirect'] = variable_get('subscription_baseurl','').'/subscription';
}

function subscription_subscription_delete($id){
  if (is_numeric($id) && $id > 0)
  {
  db_query("delete from {subscription} where id = %d",$id);
  //delete from status
  db_query("delete from {substatus} where sid = %d", $id);
  //change mail types for mails in subscription
  db_query("update {mail} set stype = ".SUBSCRIPTION_MAIL_TYPE_MANUAL." where id in (select mid from {mailsubscription} where sid = $id)");
  //delete mails from queue
  db_query("delete from {usermail} where status = 0 and mid in (select ms.mid from {mailsubscription} ms where sid = %d)", $id);
  db_query("delete from {mailsubscription} where sid = %d", $id);
  drupal_goto(variable_get('subscription_baseurl','').'/subscription');
  }
}
