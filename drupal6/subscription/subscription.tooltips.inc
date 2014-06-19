<?php

//Generates tooltip array item in JS
function _generate_tooltip_record($text, $selector, $mode){
  $text = addslashes($text);
  $text = str_replace("\n", "<br/>", $text);
  $text = str_replace("\r", "", $text);
  $result = "{ text: '$text', element: function() { return $selector; }, mode: \"$mode\" }";
  return $result;
}

//Generates tooltip data array in JS
function s_generate_tooltips(){
  $tooltips = Tooltip::getList();

  $script = "fields = Array(";
  foreach ($tooltips as $tip){
    $script .= _generate_tooltip_record($tip->text, $tip->selector, $tip->mode) . "\n, ";
  }
  if (count($tooltips) > 0)
    $script = substr($script, 0, strlen($script) - 2);
  $script .= ");";
  return $script;
}

//tooltip list
function s_tooltip_list() {
  $headers = array(
    array('data' => t('Название'), 'field' => 'title'),
    array('data' => t('Текст'), 'field' => 'text'),
    array('data' => t('Опции'), 'colspan' => 2, 'style' => 'text-align:center')
  );

  $sql = Tooltip::$SelectSQL;
  $query_count = Tooltip::$CountSQL;
  $sql .= tablesort_sql($headers);
  $limit = Tooltip::$RowsCount;
  $result = pager_query($sql, $limit, 0, $query_count);
  $data = array();
  while($resobject = db_fetch_object($result)){
    $data[] = array(
      $resobject->title,
      "<div style='width:400px; height: 150px; overflow:hidden;'>".$resobject->text."</div>",
      l(t('Редактировать'), variable_get('subscription_baseurl','')."/tooltip/$resobject->id/edit"),
      l(t('Удалить'), variable_get('subscription_baseurl','')."/tooltip/$resobject->id/delete", array('attributes' => array('onclick' => 'return confirm(\''.t('Вы действительно хотите удалить эту подсказку?').'\');'))),
    );
  }

  $output = theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;
}

function s_tooltip_delete($id){
  if (is_numeric($id) && $id > 0)
      Tooltip::delete($id);
  
  drupal_goto(variable_get('subscription_baseurl','').'/tooltip');
 
}

function s_tooltip_add_edit_form($form_state, $id='') {
  

  $form['tooltip'] = array(
    '#title' => 'Подсказа',
    '#type' => 'fieldset',
  );

  $form['tooltip']['title'] = array(
    '#title' => 'Название',
    '#type' => 'textfield',
    '#size' => 100,
    '#maxlength' => 70,
    '#required' => TRUE,
  );

  $form['tooltip']['tooltip_texteditor']['text'] = array(
    '#title' => t('Текст'),
    '#type' => 'textarea',
    '#required' => TRUE,
  );
  $form['tooltip']['tooltip_texteditor']['format'] = filter_form(FILTER_HTML_STRIP);

  $form['tooltip']['selector'] = array(
    '#title' => 'Селектор',
    '#type' => 'textfield',
    '#size' => 100,
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $modes = array('append' => 'В конец', 'prepend' => 'В начало', 'after' => 'После', 'before' => 'Перед');
  $form['tooltip']['mode'] = array(
    '#title' => 'Метод добавления',
    '#type' => 'select',
    '#options' => $modes,
    '#default_value' => 'append',
  );

  $form['tooltip']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить подсказку'),
  );

  if (is_numeric($id) && $id > 0) {
    $query = "select title, text, mode, selector from {tooltip} where id = " . $id;
    $result = db_query($query);
    $data = db_fetch_object($result);
    if ($data) {
      $form['tooltip']['id'] = array('#type' => 'value', '#value' => $id);
      $form['tooltip']['title']['#default_value'] = $data->title;
      $form['tooltip']['tooltip_texteditor']['text']['#default_value'] = $data->text;
      $form['tooltip']['selector']['#default_value'] = $data->selector;
      $form['tooltip']['mode']['#default_value'] = $data->mode;
      $form['tooltip']['submit']['#value'] = t('Сохранить изменения');
    }
  }

  return $form;
}

function s_tooltip_add_edit_form_submit($form, &$form_state) {
  $record = new stdClass();
  $record->title = $form_state['values']['title'];
  $record->text = $form_state['values']['text'];
  $record->selector = $form_state['values']['selector'];
  $record->mode = $form_state['values']['mode'];
  if ($form_state['values']['id'] == NULL) {
    drupal_write_record('tooltip', $record);
  }
  else {
    $record->id = $form_state['values']['id'];
    drupal_write_record('tooltip', $record, 'id');
  }
  $form_state['redirect'] = variable_get('subscription_baseurl','').'/tooltip';
}
