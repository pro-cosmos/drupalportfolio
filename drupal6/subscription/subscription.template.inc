<?php

function subscription_menu_template() {
  $headers = array(
    array('data' => t('Заготовка'), 'field' => 'tid', 'sort' => 'desc'),
    array('data' => t('Текст'), 'field' => 'value', 'sort' => 'desc'),
    array('data' => t('Опции'), 'colspan' => 2, 'style' => 'text-align:center')
  );

  $sql = "select id, tid, value from {template}";
  $query_count = "select count(tid) from {template}";
  $sql .= tablesort_sql($headers);
  $limit = 5;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  while($resobject = db_fetch_object($result)){
    $text = $resobject->value;
    if (is_callable('_wysiwyg_imageupload_filter_process'))
      $text = _wysiwyg_imageupload_filter_process($text);
    
    $data[] = array(
      $resobject->tid,
      "<div style='width:400px; height: 150px; overflow:hidden;'>".$text."</div>",
      l(t('Редактировать'), variable_get('subscription_baseurl','')."/template/$resobject->id/edit"),
      l(t('Удалить'), variable_get('subscription_baseurl','')."/template/$resobject->id/delete", array('attributes' => array('onclick' => 'return confirm(\''.t('Вы действительно хотите удалить эту заготовку?').'\');'))),
    );
  }

  $output = theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;
}

function subscription_delete_template($id){
  if (is_numeric($id) && $id > 0) 
  {
  $sql = "delete from {template} where id = %d";
  db_query($sql, $id);
  drupal_goto(variable_get('subscription_baseurl','').'/template');
  }
  
}

function subscription_template_add_edit_form($form_state, $id='') {

  $form['template'] = array(
    '#title' => t('Добавление заготовки'),
    '#type' => 'fieldset',
  );

  $form['template']['tid'] = array(
    '#title' => t('Заготовка'),
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 30,
    '#description' => 'Ввведите Код заготовки. Он сможет быть использован в письме как {заготовка}',
    '#required' => TRUE,
  );

  $form['template']['ttexteditor']['ttext'] = array(
    '#title' => 'Текст',
    '#type' => 'textarea',
    '#description' => 'Введите Текст заготовки. Он будет вставлен вместо {заготовка} при отправке письма',
    '#required' => TRUE,
  );
  $form['template']['ttexteditor']['format'] = filter_form(1); // filtered HTML format
  

  $form['template']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить заготовку'),
  );

  if (is_numeric($id) && $id > 0) {
    $query = "select tid, value from {template} where id = " . $id;
    $result = db_query($query);
    $data = db_fetch_object($result);
    if ($data) {
      $form['template']['id'] = array('#type' => 'value', '#value' => $id);
      $form['template']['tid']['#default_value'] = $data->tid;
      $form['template']['ttexteditor']['ttext']['#default_value'] = $data->value;
      $form['template']['submit']['#value'] = t('Сохранить изменения');
    }
  }

  return $form;
}

function subscription_template_add_edit_form_validate($form, &$form_state) {
  if ($form_state['values']['id'] == NULL) {
    $query = "select id from {template} where tid = '".$form_state['values']['tid']."'";
    $result = db_query($query);
    if (db_fetch_object($result) != FALSE) {
      form_error($form['template']['tid'], t('Шаблон с таким кодом уже существует.'));
    }
  }
}

function subscription_template_add_edit_form_submit($form, &$form_state) {
  $record = new stdClass();
  $record->tid = $form_state['values']['tid'];
  $record->value = $form_state['values']['ttext'];
  if ($form_state['values']['id'] == NULL) {
    drupal_write_record('template', $record);
  }
  else {
    $record->id = $form_state['values']['id'];
    drupal_write_record('template', $record, 'id');
  }
  $form_state['redirect'] = variable_get('subscription_baseurl','').'/template';
}
