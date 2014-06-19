<?php
require_once(drupal_get_path('module', 'subscription') . "/inc/subscription_mail/subscription.mail.inc");

function s_create_confirm_attribute($message){
  return array('attributes' => array('onclick' => "return confirm('$message');"));
}

function s_catalog_towns_list(){

  $headers = array(
    array('data' => 'Название', 'field' => 'title'),
    array('data' => 'Область', 'field' => 'state'),
    array('data' => 'Опции', 'colspan' => 2, 'style' => 'text-align:center')
  );

  $limit = 50;
  $count_sql = "select count(*) from {town}";
  $select_sql = "select t.id, t.title, s.title as state, (select count(*) from {ori_user} u where u.tid = t.id) as used_count from {town} t inner join {state} s on s.id = t.sid";
  $select_sql .= tablesort_sql($headers);

  $result = pager_query($select_sql, $limit, 0, $count_sql);
  while($obj = db_fetch_object($result)){
    $obj_data = array(
        $obj->title,
        $obj->state,
        l('Редактировать', "admin/subscription/catalog/towns/$obj->id/edit"),
      );
    if ($obj->used_count == 0){
      $obj_data[] = l('Удалить', "admin/subscription/catalog/towns/$obj->id/delete", s_create_confirm_attribute('Вы действительно хотите удалить этот город?'));
    }
    $data[] = array('id' => "row-$obj->id", 'data' => $obj_data);
  }

  $output = "<div>".l('Добавить', "admin/subscription/catalog/towns/add")."</div>";
  $output .= theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;
}

function s_catalog_states_list(){

  $headers = array(
    array('data' => 'Название', 'field' => 'title'),
    array('data' => 'Страна', 'field' => 'country'),
    array('data' => 'Опции', 'colspan' => 2, 'style' => 'text-align:center')
  );

  $limit = 20;
  $count_sql = "select count(*) from {state}";
  $select_sql = "select s.id, s.title, c.title as country, (select count(*) from {ori_user} u where u.stid = s.id) as used_count from {state} s inner join {country} c on c.id = s.cid";
  $select_sql .= tablesort_sql($headers);

  $result = pager_query($select_sql, $limit, 0, $count_sql);
  while($obj = db_fetch_object($result)){
    $obj_data = array(
        $obj->title,
        $obj->country,
        l('Редактировать', "admin/subscription/catalog/states/$obj->id/edit"),
      );
    if ($obj->used_count == 0){
      $obj_data[] = l('Удалить', "admin/subscription/catalog/states/$obj->id/delete", s_create_confirm_attribute('Вы действительно хотите удалить эту область?'));
    }
    $data[] = array('id' => "row-$obj->id", 'data' => $obj_data);
  }

  $output = "<div>".l('Добавить', "admin/subscription/catalog/states/add")."</div>";
  $output .= theme('table', $headers, $data);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;
}

function s_catalog_countries_list(){

  $headers = array(
    array('data' => '', 'field' => 'c.weight'),
    array('data' => 'Название', 'field' => 'title'),
    array('data' => 'Код', 'field' => 'code'),
    array('data' => 'Опции', 'colspan' => 2, 'style' => 'text-align:center')
  );

  $select_sql = "select c.id, c.title, c.code, c.weight, (select count(*) from {ori_user} u where u.cid = c.id) as used_count from {country} c";
  $select_sql .= tablesort_sql($headers);
  foreach(DB::fetch_array($select_sql) as $obj){
    $obj_data = array(
        array('class' => 'weight-holder', 'data' => "<span type=\"text\" class=\"weight-holder\" weight=\"$obj->weight\"/>"),
        $obj->title,
        $obj->code,
        l(t('Edit'), "admin/subscription/catalog/countries/$obj->id/edit"),
      );
    if ($obj->used_count == 0){
      $obj_data[] = l(t('Delete'), "admin/subscription/catalog/countries/$obj->id/delete", s_create_confirm_attribute('Вы действительно хотите удалить эту страну?'));
    }
    $data[] = array('id' => "row-$obj->id", 'data' => $obj_data);
  }

  $output = "<div>".l('Добавить', "admin/subscription/catalog/countries/add")."</div>";
  $output .= theme('table', $headers, $data);

  $output .= "<a href='/admin/subscription/catalog/countries/changeorder' id='change-countries-weight-button'>[Сохранить новый порядок]</a></div>";

  drupal_add_js(drupal_get_path('module', 'subscription'). "/js/subscription.catalog.list.js");
  drupal_add_js(drupal_get_path('module', 'subscription') ."/inc/subscription_mail/jquery.tablednd_0_5.js");

  return $output;
}

function s_catalog_countries_changeorder(){
  $order = $_GET['order'];
  $order = explode('&', $order);
  $new_order = array();
  $weight = 0;
  foreach($order as $o){
    $id = explode('=', $o);
    if (!isset($id[1]))
      continue;
    $id = $id[1];
    if ($id == NULL or $id == ''){
      continue;
    }
    db_query("update {country} set weight = $weight where id = $id");
    $weight++;
  }
  print drupal_to_js(array('status' => 'ok'));
}

function subscription_catalog_list(){
  $output = "<ul>";
  $output.= '    <li>'.l('Страны', "admin/subscription/catalog/countries").'</li>';
  $output.= '    <li>'.l('Области', "admin/subscription/catalog/states").'</li>';
  $output.= '    <li>'.l('Города', "admin/subscription/catalog/towns").'</li>';
  $output.='</ul>';
  $output .= drupal_get_form('subscription_catalog_load_form', 'country');
  return $output;
}

function subscription_catalog_load_form(){
  $form['w'] = array(
    '#type' => 'fieldset',
    '#title' => 'Загрузить страны, области и города из TXT документа',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
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

function s_load_add_country($title, $code){
  $id = DB::fetch_result("select id from {country} where title like '$title'");
  $country = new stdClass();
  $country->title = $title;
  $country->code = $code;
  if ($id){
    $country->id = $id;
    drupal_write_record('country', $country, 'id');
  }
  else{
    drupal_write_record('country', $country);
  }
  return $country->id;
}

function s_load_add_state($title, $country_id){
  $id = DB::fetch_result("select id from {state} where title like '$title'");
  $state = new stdClass();
  $state->title = $title;
  $state->cid = $country_id;
  if ($id){
    $state->id = $id;
    drupal_write_record('state', $state, 'id');
  }
  else{
    drupal_write_record('state', $state);
  }
  return $state->id;
}

function s_load_add_town($title, $state_id){
  $id = DB::fetch_result("select id from {town} where title like '$title'");
  $town = new stdClass();
  $town->title = $title;
  $town->sid = $state_id;
  if ($id){
    $town->id = $id;
    drupal_write_record('town', $town, 'id');
  }
  else{
    drupal_write_record('town', $town);
  }
  return $town->id;
}
function s_dump($msg){
  drupal_set_message(var_dump($msg));
}

function subscription_catalog_load_form_submit($form, &$form_state){
  $state = 'c';

  $cid = 0;
  $sid = 0;

  if ($file = file_save_upload('file')){
    $data = file_get_contents($file->filepath);
    $data = iconv('cp1251', 'utf-8', $data);
    str_replace("\r\n", "\n", $data);
    $values = explode("\n", $data);
    foreach($values as $value){
      $value = trim($value);
      if ($value == ''){
        continue;
      }
      else if ($value == '#c'){
          $state = 'c';
      }
      else if ($value == '#s'){
          $state = 's';
      }
      else if ($value == '#t'){
          $state = 't';
      }
      else{
        switch($state){
          case 'c': {
            list($title, $code) = split(" : ", $value);
            $cid = s_load_add_country(trim($title), trim($code));
            break;
          }
          case 's': {
            if ($cid == NULL){
              drupal_set_message("Область '$value' не может быть добавлена. Нет страны.", 'error');
              return;
            }
            $sid = s_load_add_state($value, $cid);
            break;
          }
          case 't': {
            if ($sid == NULL){
              drupal_set_message("Город '$value' не может быть добавлен. Нет области.", 'error');
              return;
            }
            s_load_add_town($value, $sid);
            break;
          }
        }
      }
    }
  }
  $form_state['redirect'] = "admin/subscription/catalog";
}

function s_catalog_item_add_edit_page($catalog, $id = NULL){
  $output = drupal_get_form('s_catalog_' . $catalog. '_add_edit', $id);
  $output .= "<div><a href='/admin/subscription/catalog/$catalog'>[Вернуться к списку]</a></div>";
  return $output;
}

function s_catalog_countries_add_edit($form_state, $id){
  $form['fs'] = array(
    '#title' => 'Добавление страны:',
    '#type' => 'fieldset',
  );

  $form['fs']['title'] = array(
    '#title' => 'Название',
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['fs']['code'] = array(
    '#title' => 'Код страны',
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить страну'),
  );

  if ($id) {
    $country = Country::get($id);
    $form['fs']['#title'] = 'Редактирование страны:' . $country->title;
    $form['fs']['id'] = array('#type' => 'value', '#value' => $id);
    $form['fs']['title']['#default_value'] = $country->title;
    $form['fs']['code']['#default_value'] = $country->code;
    $form['fs']['submit']['#value'] = t('Сохранить изменения');
  }

  return $form;
}

function s_catalog_countries_add_edit_submit($form, &$form_state) {
  $values = $form_state['values'];

  $country = new stdClass();
  $country->title = $values['title'];
  $country->code = $values['code'];
  if ($values['id']){
    $country->id = $values['id'];
    drupal_write_record('country', $country, 'id');
  }
  else{
    drupal_write_record('country', $country);
  }

  $form_state['redirect'] = "admin/subscription/catalog/countries";
}

function s_catalog_states_add_edit($form_state, $id){
  $form['fs'] = array(
    '#title' => 'Добавление области:',
    '#type' => 'fieldset',
  );

  $form['fs']['title'] = array(
    '#title' => 'Название',
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $countries = Country::select_list();
  $countries[-1] = '';
  $form['fs']['country'] = array(
    '#title' => 'Страна',
    '#type' => 'select',
    '#options' => $countries,
    '#default_value' => -1,
    '#required' => TRUE,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить область'),
  );

  if ($id) {
    $state = State::get($id);
    $form['fs']['#title'] = 'Редактирование области:' . $state->title;
    $form['fs']['id'] = array('#type' => 'value', '#value' => $id);
    $form['fs']['title']['#default_value'] = $state->title;
    $form['fs']['country']['#default_value'] = $state->cid;
    $form['fs']['submit']['#value'] = t('Сохранить изменения');
  }

  return $form;
}

function s_catalog_states_add_edit_submit($form, &$form_state) {
  $values = $form_state['values'];

  $state = new stdClass();
  $state->title = $values['title'];
  $state->cid = $values['country'];
  if ($values['id']){
    $state->id = $values['id'];
    drupal_write_record('state', $state, 'id');
  }
  else{
    drupal_write_record('state', $state);
  }

  $form_state['redirect'] = "admin/subscription/catalog/states";
}

function s_catalog_towns_add_edit($form_state, $id){
  $form['fs'] = array(
    '#title' => 'Добавление области:',
    '#type' => 'fieldset',
  );

  $form['fs']['title'] = array(
    '#title' => 'Название',
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $states = State::select_list();
  $states[-1] = '';
  $form['fs']['state'] = array(
    '#title' => 'Область',
    '#type' => 'select',
    '#options' => $states,
    '#default_value' => -1,
    '#required' => TRUE,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить область'),
  );

  if ($id) {
    $town = Town::get($id);
    $form['fs']['#title'] = 'Редактирование области:' . $town->title;
    $form['fs']['id'] = array('#type' => 'value', '#value' => $id);
    $form['fs']['title']['#default_value'] = $town->title;
    $form['fs']['state']['#default_value'] = $town->sid;
    $form['fs']['submit']['#value'] = t('Сохранить изменения');
  }

  return $form;
}

function s_catalog_towns_add_edit_submit($form, &$form_town) {
  $values = $form_town['values'];

  $town = new stdClass();
  $town->title = $values['title'];
  $town->sid = $values['state'];
  if ($values['id']){
    $town->id = $values['id'];
    drupal_write_record('town', $town, 'id');
  }
  else{
    drupal_write_record('town', $town);
  }

  $form_town['redirect'] = "admin/subscription/catalog/towns";
}

function s_catalog_countries_delete($id){
  DB::delete('country', 'id', $id);
  foreach(State::listByCountry($id) as $state){
    DB::delete('state', 'id', $state->id);
    DB::delete('town', 'sid', $state->id);
  }
  drupal_goto("admin/subscription/catalog/countries");
}

function s_catalog_states_delete($id){
  DB::delete('state', 'id', $id);
  DB::delete('town', 'sid', $state->id);
  drupal_goto("admin/subscription/catalog/states");
}

function s_catalog_towns_delete($id){
  DB::delete('town', 'id', $id);
  drupal_goto("admin/subscription/catalog/towns");
}
