<?php

function _pms_userform_add_js($plugin, $path = '') {

  $path = $path ? $path : drupal_get_path('module', 'pms_userform');
  $args = func_get_args();

  //common
  drupal_add_js($path . '/js/autocomplete/jquery.ui.core.min.js');
  drupal_add_js($path . '/js/autocomplete/jquery.ui.widget.min.js');
  drupal_add_js($path . '/js/autocomplete/jquery.ui.position.min.js');

  switch ($plugin) {
    case 'autocomplete':
      drupal_add_js($path . '/js/autocomplete/jquery.ui.autocomplete.min.js');
      drupal_add_css($path . '/js/autocomplete/jquery.autocomplete.css');
      drupal_add_css($path . '/js/autocomplete/themes/base/jquery.ui.base.css');
      drupal_add_css($path . '/js/autocomplete/themes/base/jquery.ui.theme.css');
      break;

    case 'datapicker':
      $currCountry = $args[2];
      drupal_add_js($path . '/js/datapicker/languages/jquery.ui.datepicker-' . $currCountry . '.js');
      drupal_add_js($path . '/js/datapicker/jquery.ui.datepicker.js');
      drupal_add_css($path . '/js/datapicker/themes/base/jquery.ui.base.css');
      drupal_add_css($path . '/js/datapicker/themes/base/jquery.ui.datepicker.css');
      break;

    case 'formvalidate':
      $currCountry = $args[2];
      drupal_add_js($path . '/js/formvalidate/js/languages/jquery.validationEngine-' . $currCountry . '.js');
      drupal_add_js($path . '/js/formvalidate/js/jquery.validationEngine.js');
      drupal_add_css($path . '/js/formvalidate/validationEngine.jquery.css');
      drupal_add_css($path . '/js/formvalidate/template.css');
      break;

    case 'cluetooltip':
      drupal_add_js($path . '/js/cluetip/jquery.cluetip.min.js');
      drupal_add_css($path . '/js/cluetip/jquery.cluetip.css');
      break;
  }
}

/**
 *
 * @param type $form_state
 * @return string
 */
function pms_userform_regform(&$form_state) {

// код страны автоматически определяемы модулем geo
  //$GEO = geoip_record_by_name($_SERVER['REMOTE_ADDR']);
  $GEOIP_COUNTRY = variable_get('pms_users_default_country_id', 1);
  $Country = isset($form_state['post']['country']) ?
      Country::get($form_state['post']['country'], 'geocode') :
      Country::get($GEOIP_COUNTRY);


  $path = drupal_get_path('module', 'pms_userform');
  //autocomplete
  _pms_userform_add_js('autocomplete', $path);
  //datapicker
  _pms_userform_add_js('datapicker', $path, $Country->geocode);
  //jquery form validate
  _pms_userform_add_js('formvalidate', $path, $Country->geocode);
  //clue tooltip
  _pms_userform_add_js('cluetooltip', $path);



  $reqstar = '<span class="form-required" title="Обязательное поле">*</span>';


  //name
  $form['lastname'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Lastname') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Lastname'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['lastname'] ? $form_state['post']['lastname'] : 'test')
  );
  $form['firstname'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Firstname') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Firstname'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['firstname'] ? $form_state['post']['firstname'] : 'firstname')
  );
  $form['middlename'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Middlename') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Middlename'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['middlename'] ? $form_state['post']['middlename'] : 'Отчество')
  );

  $form['sex'] = array(
    '#prefix' => "<tr id='tr_sex'><td class='row-label required'>" . t('Sex') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'radios',
    '#title' => t('Sex'),
    '#default_value' => (isset($form_state['post']['sex']) ? $form_state['post']['sex'] : -1),
    '#options' => array(t('Female'), t('Male')),
    '#required' => TRUE,
    '#theme' => 'radios_sex'
  );


  $form['birthday'] = array(
    '#prefix' => "<tr  id='tr_birthday'><td class='row-label required'>" . t('Birthday') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#title' => t('Birthday'),
    '#date_format' => 'd/m/Y',
    '#default_value' => date('d/m/Y', $form_state['post']['birthday']),
    '#required' => TRUE,
    '#size' => 10,
    '#maxlength' => 10,
  );

  //passport
  $form['series'] = array(
    '#prefix' => "<tr  id='tr_series'><td class='row-label required'>" . t('Passport') . ' - ' . t('series') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Passport') . ' - ' . t('series'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['series'] ? $form_state['post']['series'] : rand(1, 100))
  );

  $form['number'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Passport') . ' - ' . t('number') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Passport') . ' - ' . t('number'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['number'] ? $form_state['post']['number'] : rand(1, 100))
  );


//country
  $country_options = Country::select_list();
  $country_options[-1] = t('Any');
  $form['country'] = array(
    '#prefix' => "<tr id='tr_country'><td class='row-label required'>" . t('Country') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Country'),
    '#type' => 'select',
    '#options' => $country_options,
    '#default_value' => $Country->id,
    '#required' => TRUE,
  );

  $phonecodesCountry = Country::select_list('geocode', 'code');
  $currCountryPhone = $phonecodesCountry[$Country->geocode];

  //ADD COMMON VARS
  $tmp = array();
  foreach ($phonecodesCountry as $k => $v) {
    $tmp[] = '"' . $k . '":"' . $v . '"';
  }
  $js_inline = "PMS_USERFORM_COUNTRY_CODES = {" . implode(', ', $tmp) . "};";
  drupal_add_js($js_inline, 'inline');


  $form['index'] = array(
    '#prefix' => "<tr id='tr_index' ><td class='row-label required'>" . t('Index') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Index'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => (isset($form_state['post']['pindex']) ? $form_state['post']['pindex'] : '')
  );

  $form['table_inner_start'] = array(
    '#type' => 'markup',
    '#value' => "<tr class='tr_inner'><td colspan=2><table class='table_inner'>"
  );


  if ($form_state['post']['index']) {
    $curr_cacheIndex = PMS_userform::init($Country->geocode)->getSelect('index', array('search' => $form_state['post']['index']));
    $curr_cacheIndex = $curr_cacheIndex[0];
  }

  if ($Country)
    $state_options = State::init($Country->geocode)->select_list();

  $state_options[-1] = t('Any');
  //$state_options = State::select_list();

  $form['state'] = array(
    '#prefix' => "<tr id='tr_state' ><td class='row-label required'>" . t('State/Edge') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('State/Edge'),
    '#type' => 'select',
    '#options' => $state_options,
    '#default_value' => empty($form_state['post']['state_hidden']) ? '' : (int) $form_state['post']['state_hidden']
  );
  $form['state_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => (int) $form_state['post']['state_hidden']
  );


  $communityType_options = array(0 => 'Город', 1 => 'Другое...');
  $communityType_value = isset($form_state['post']['communityType_hidden']) ? (int) $form_state['post']['communityType_hidden'] : -1;

  $form['communityType'] = array(
    '#prefix' => "<tr id='tr_communityType' ><td class='row-label required'>" . t('Place type') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Place type'),
    '#type' => 'radios',
    '#options' => $communityType_options,
    '#default_value' => $communityType_value
      //'#required' => TRUE
  );

  $form['communityType_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => $communityType_value
  );

  //if($communityType_hidden == 0){
  $city_class = $communityType_value == 1 ? 'class="hidden"' : '';
  $form['town'] = array(
    '#prefix' => "<tr id='tr_town' " . $city_class . "><td class='row-label required'>" . t('City') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => ($communityType_value == 1) ? '' : $form_state['post']['town']
      // '#required' => TRUE,
  );
  $form['town_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => ($communityType_value == 1) ? '' : $form_state['post']['town_hidden']
  );
  //}

  $district_class = ($communityType_value) ? '' : 'class="hidden"';
  $form['district'] = array(
    '#prefix' => "<tr id='tr_district' " . $district_class . "><td class='row-label required'>" . t('District') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('District'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#default_value' => $form_state['post']['district']
      // '#required' => TRUE,
  );
  $form['district_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => $form_state['post']['district_hidden']
  );


  $place_class = ($communityType_value) ? '' : 'class="hidden"';
  $form['place'] = array(
    '#prefix' => "<tr id='tr_place' " . $place_class . "><td class='row-label required'>" . t('Place') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Place'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#default_value' => $form_state['post']['loctitle'],
      // '#required' => TRUE,
  );
  $form['place_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => $form_state['post']['place_hidden']
  );


  $form['table_inner_end'] = array(
    '#type' => 'markup',
    '#value' => "</table></td></tr>"
  );



  $form['street'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Street') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Street'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['street'] ? $form_state['post']['street'] : '')
  );
  $form['house'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('House') . $reqstar . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('House'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['house'] ? $form_state['post']['house'] : '')
  );
  $form['houseblock'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Houseblock') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Houseblock'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => ($form_state['post']['houseblock'] ? $form_state['post']['houseblock'] : '')
  );
  $form['flat'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Flat') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Flat'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => ($form_state['post']['flat'] ? $form_state['post']['flat'] : '')
  );
  //phone
  $prefix = "<tr><td class='row-label'>" . t('Home phone') . "</td><td><table><tr><td>" . t('Country code') . "</td><td>" . t('Prefix') . "</td><td>" . t('Phone number') . "</td></tr>" .
      "<tr>";

  $form['tccode'] = array(
    '#prefix' => $prefix . "<td class='td-value'>+",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => $currCountryPhone
  );
  $form['tcode'] = array(
    '#prefix' => "<td class='td-value'>",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => ($form_state['post']['tcode'] ? $form_state['post']['tcode'] : '')
  );
  $form['tnumber'] = array(
    '#prefix' => "<td class='td-value'>",
    '#suffix' => '</td></tr></table></td></tr>',
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 20,
    '#default_value' => ($form_state['post']['tnumber'] ? $form_state['post']['tnumber'] : '')
  );

  $prefix = "<tr class='m-phone'><td class='row-label'>" . t('Mobile phone') . $reqstar . "</td><td><table><tr class='m-phone-desc'><td>" . t('Country code') . "</td><td>" . t('Prefix') . theme('ori_tooltiplink', 4) . "</td><td>" . t('Phone number') . theme('ori_tooltiplink', 5) . "</td></tr>" .
      "<tr>";
  $form['mccode'] = array(
    '#prefix' => $prefix . "<td class='td-value'>+",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => $currCountryPhone
  );
  $form['mprefix'] = array(
    '#prefix' => "<td class='td-value'>(",
    '#suffix' => ')</td>',
    '#type' => 'textfield',
    '#title' => 'Prefix',
    '#size' => 6,
    '#maxlength' => 6,
    '#attributes' => array('class' => 'validate[required] text-input'),
    // '#required' => TRUE,
    '#default_value' => ($form_state['post']['mprefix'] ? $form_state['post']['mprefix'] : '')
  );
  $form['mnumber'] = array(
    '#prefix' => "<td class='td-value'>",
    '#suffix' => '</td></tr></table></td></tr>',
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 20,
    '#attributes' => array('class' => 'validate[required] text-input'),
    // '#required' => TRUE,
    '#default_value' => ($form_state['post']['mnumber'] ? $form_state['post']['mnumber'] : '')
  );
  //email
  $form['email'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Email') . $reqstar . theme('ori_tooltiplink', 3) . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => 'Email',
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes' => array('class' => 'validate[required] text-input'),
    '#default_value' => ($form_state['post']['email'] ? $form_state['post']['email'] : '')
  );


  //registration purpose
  $purpose_types = variable_get('constant_purposes', array());
  $purptype_default = ($form_state['post']['purposeType']>=0) ? $form_state['post']['purposeType'] : -1;
  $form['purposeType'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Registration purpose') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Registration purpose'),
    '#type' => 'radios',
    '#options' => $purpose_types,
    '#default_value' => $purptype_default,
  );

  ;
  $form['pother'] = array(
    '#prefix' => "<tr id='tr_pother' " . ($purptype_default == 3 ? "" : "class='hidden'") . "><td class='row-label required'></td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textarea',
    '#description' => t('Describe your purpose'),
    '#default_value' => empty($form_state['post']['pother']) ? "" : $form_state['post']['pother']
  );

  //additional
  $form['profession'] = array(
    '#prefix' => "<tr><td class='row-label required'>" . t('Your profession') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Profession'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    // '#required' => TRUE,
    '#default_value' => empty($form_state['post']['profession']) ? "" : $form_state['post']['profession']
  );



  //add common
  $setting = array(
    'def_country' => $Country,
    'post_index' => $form_state['post']['index'],
    'post_state' => $form_state['post']['state_hidden'],
    'curr_cacheIndex' => $curr_cacheIndex,
    'baseurl' => url(variable_get('pms_userform_baseurl', ''))
  );

  drupal_add_js($path . '/js/pms_userform.js', 'module');
  drupal_add_js(array('pms_Userform' => $setting), 'setting');
  drupal_add_css($path . '/css/pms_userform.css');

  $form['#redirect'] = false; //false;


  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 */
function pms_userform_regform_validate($form, &$form_state) {
  //regform_validate
  /* $resp = OriClient::init()->validateUser($form_state['values']);
    if ($resp['errors'] && is_array($resp['errors'])){
    foreach($resp['errors'] as $k=>$error){
    if (is_array($error)){
    list($field,$text) = each($error);
    }
    else{
    $field = ''; $text = $error;
    }

    form_set_error($field, $text);
    }
    }
   */
}

/**
 *  Ajax callback manager
 * @param type $callback_name
 * @return type
 */
function pms_userform_ajax_callback($callback_name = '') {

  $callback_function = __FUNCTION__ . '_' . $callback_name;
  if (function_exists($callback_function))
    echo $callback_function();
  else
    echo '{error: cant find callback}';
}

/**
 *  Ajax callback for postindex autocomplete fields
 */
function pms_userform_ajax_callback_getselect() {

  // тип поля autocomplete
  $field = $_POST['field'];
  // ключ поиска
  // код страны по которой ищем
  $cid = check_plain($_POST['country']);
  $country = Country::get($cid);

  if (!empty($field)) {
    $result = PMS_userform::init($country->geocode)->getSelect($field);

    return drupal_json(
            array('result' => $result,
              'count' => count($result))
    );
  }
}

/**
 *  Ajax callback for postindex autocomplete fields
 */
function pms_userform_ajax_callback_autocomplete() {
  // тип поля autocomplete
  $field = $_POST['field'];
  // код страны по которой ищем
  $cid = check_plain($_POST['country']);
  $country = Country::get($cid);
  $arg = $_POST;

  if (!empty($field)) {
    $result = PMS_userform::init($country->geocode)->getSelect($field, $arg);

    return drupal_json(
            array('result' => $result,
              'count' => count($result))
    );
  }
}