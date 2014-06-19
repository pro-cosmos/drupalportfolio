<?php
include_once('ori_regform.factory.php');



/**
 *
 * @param type $form_state
 * @return string
 */
function ori_regform_regform(&$form_state){
  drupal_set_html_head('<meta name="robots" content="noindex" />');

    // код страны автоматически определяемы модулем geo
    //$GEO = geoip_record_by_name($_SERVER['REMOTE_ADDR']);

 $GEOIP_COUNTRY ='ru';
 $currCountry = empty($form_state['post']['country'])? $GEOIP_COUNTRY : $form_state['post']['country'];
 $path = drupal_get_path('module', 'ori_regform');


  $reqstar = '<span class="form-required" title="Обязательное поле">*</span>';
  $form['table_start'] = array(
    '#type' => 'markup',
    '#value' => "<table class='user-table'>"
  );


  //name
  $form['lastname'] = array(
    '#prefix' => "<tr id='tr_lastname'><td class='row-label required'>".t('Lastname').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Lastname'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input') ,
    '#default_value' => ($form_state['post']['lastname']? $form_state['post']['lastname'] : '')
  );
  $form['firstname'] = array(
    '#prefix' => "<tr><td class='row-label required'>".t('Firstname').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Firstname'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input')  ,
    '#default_value' => ($form_state['post']['firstname']? $form_state['post']['firstname'] : '')
  );
  $form['middlename'] = array(
    '#prefix' => "<tr><td class='row-label required'>".t('Middlename').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Middlename'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input')  ,
     '#default_value' => ($form_state['post']['middlename']? $form_state['post']['middlename'] : '')
  );

  $form['sex'] = array(
  '#prefix' => "<tr id='tr_sex'><td class='row-label required'>".t('Sex').$reqstar."</td><td class='td-value'>",
  '#suffix' => '</td></tr>',
  '#type' => 'radios',
  '#title' => t('Sex'),
  '#default_value' => (isset($form_state['post']['sex'])? $form_state['post']['sex'] : -1),
  '#options' => array(t('Жен.'), t('Муж.')),
  '#required' => TRUE,
  '#theme' => 'radios_sex'
  );


  $form['birthday'] = array(
        '#prefix' => "<tr  id='tr_birthday'><td class='row-label required'>".t('Birthday').$reqstar."</td><td class='td-value'>",
        '#suffix' => '</td></tr>',
        '#type' => 'textfield',
        '#title' =>  t('Birthday'),
        '#date_format' => 'd/m/Y',
        '#default_value' => ($form_state['post']['birthday']? $form_state['post']['birthday'] : ''), //date("d/m/Y",time() - 24 * 60 * 60*365*18)), //ОТ 18 ЛЕТ
        '#required' => TRUE,
        '#size' => 10,
        '#maxlength' => 10,
        '#attributes'=>array('class'=>'validate[required] text-input datepicker')
      );

  //passport
  $form['series'] = array(
    '#prefix' => "<tr  id='tr_series'><td class='row-label required'>".t('Passport - series').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Passport - series'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input'),
    '#default_value' => ($form_state['post']['series']? $form_state['post']['series']: '')
  );

  $form['number'] = array(
    '#prefix' => "<tr><td class='row-label required'>".t('Passport - number').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Passport - number'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input') ,
    '#default_value' => ($form_state['post']['number']? $form_state['post']['number']: '')
  );


//country
  $country_data = OriRegform::init()->getSelect('country');
  $country_options = empty($country_data['result'])? array() : $country_data['result'];
  $form['country'] = array(
    '#prefix' => "<tr id='tr_country'><td class='row-label required'>Страна".$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Страна'),
    '#type' => 'select',
    '#options' => array_merge($country_options, array(0 => t('Any'))),
    '#default_value' => $currCountry,
    '#required' => TRUE,
  );

  $country_phonecodes = empty($country_data['phonecodes'])? array() : $country_data['phonecodes'];
  $currCountryPhone = empty($country_phonecodes[$currCountry])? '':  $country_phonecodes[$currCountry];

  //add commons vars
  $tmp = array();
  foreach($country_phonecodes as $k=>$v){
   $tmp[]= '"'.$k.'":"'.$v.'"';
  }
  $js_inline = " ORI_REGFORM_COUNTRY_CODES = {".implode(', ',$tmp)."};";
  drupal_add_js($js_inline, 'inline');


  $form['index'] = array(
    '#prefix' => "<tr id='tr_index' ><td class='row-label required'>".t('Postindex').$reqstar.theme('ori_tooltiplink', 7)."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Postindex'),
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input'),
    '#default_value' => (isset($form_state['post']['pindex'])? $form_state['post']['pindex'] : '')
     );


   $form['table_inner_start'] = array(
    '#type' => 'markup',
    '#value' => "<tr class='tr_inner'><td colspan=2><table class='table_inner txtdisable'>"
    );


 if($form_state['post']['index'])
   {
     $curr_cacheIndex = OriRegform::init($form_state['post']['country'])->getSelect('index', array('search'=>$form_state['post']['index']));
     $curr_cacheIndex = $curr_cacheIndex[0];
  }

  $state_options = array(''=>t('Any'));
  if($form_state['post']['state_hidden']){
    $state_data = OriRegform::init($currCountry)->getSelect('state');
    foreach($state_data as $el){
          $state_options[$el['id']]=$el['title'];
    }
  }

  $form['state'] = array(
    '#prefix' => "<tr id='tr_state' ><td class='row-label required'>".t('State/Edge').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('State/Edge'),
    '#type' => 'select',
    '#options' => $state_options,
    '#default_value'  => empty($form_state['post']['state_hidden'])? '': $form_state['post']['state_hidden'],
    '#validated' =>true,
    //'#required' => TRUE
  );
    $form['state_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => (int)$form_state['post']['state_hidden']
  );



  $communityType_options = array(0=>'Город', 1=>'Другое (деревня, посёлок, п.г.т.)');
  $communityType_value   = isset($form_state['post']['communityType_hidden'])? (int)$form_state['post']['communityType_hidden'] : -1;

  $form['communityType'] = array(
    '#prefix' => "<tr id='tr_communityType' ><td class='row-label required'>".t('Place type')."</td><td class='td-value'>",
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


  $form['town'] = array(
    '#prefix' => "<tr id='tr_town' class='hidden'><td class='row-label required'>".t('City').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => $form_state['post']['town']
   // '#required' => TRUE,
  );
  $form['town_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => 0
  );



    $form['district'] = array(
    '#prefix' => "<tr id='tr_district' class='hidden'><td class='row-label required'>".t('District').$reqstar."</td><td class='td-value'>",
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

  $form['place'] = array(
    '#prefix' => "<tr id='tr_place' class='hidden'><td class='row-label required'>".t('Place').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Place'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#default_value' => '',
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
    '#prefix' => "<tr><td class='row-label required'>".t('Street').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Street'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
   // '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input'),
    '#default_value' => ($form_state['post']['street']? $form_state['post']['street']: '')
  );
  $form['house'] = array(
    '#prefix' => "<tr><td class='row-label required'>".t('House').$reqstar."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('House'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input'),
    '#default_value' => ($form_state['post']['house']? $form_state['post']['house']: '')
  );
  $form['houseblock'] = array(
    '#prefix' => "<tr><td class='row-label'>".t('House block')."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('House block'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => ($form_state['post']['houseblock']? $form_state['post']['houseblock']: '')
  );
  $form['flat'] = array(
    '#prefix' => "<tr><td class='row-label'>".t('Flat')."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Flat'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => ($form_state['post']['flat']? $form_state['post']['flat']: '')
  );

  $prefix = "<tr class='m-phone'><td class='row-label'>".t('Mobile phone').$reqstar."</td><td><table><tr class='m-phone-desc'><td>".t('Country code')."</td><td>".t('Prefix').theme('ori_tooltiplink', 4)."</td><td>".t('Phone number').theme('ori_tooltiplink', 5)."</td></tr>".
            "<tr>";
  $form['mccode'] = array(
    '#prefix' => $prefix."<td class='td-value'>+",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#attributes'=>array('class'=>'validate[required] text-input', 'readonly'=>'readonly'),
    '#default_value' => $currCountryPhone
  );
  $form['mprefix'] = array(
    '#prefix' => "<td class='td-value'>(",
    '#suffix' => ')</td>',
    '#type' => 'textfield',
    '#title' => t('Prefix'),
    '#size' => 6,
    '#maxlength' => 6,
'#attributes'=>array('class'=>'validate[required] text-input'),
   // '#required' => TRUE,
   '#default_value' => ($form_state['post']['mprefix']? $form_state['post']['mprefix']: '')
  );
  $form['mnumber'] = array(
    '#prefix' => "<td class='td-value'>",
    '#suffix' => '</td></tr></table></td></tr>',
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 20,
   // '#required' => TRUE,
 '#attributes'=>array('class'=>'validate[required] text-input'),
    '#default_value' => ($form_state['post']['mnumber']? $form_state['post']['mnumber']: '')
  );

  //phone
  $prefix = "<tr class='m-phone'><td class='row-label'>".t('Home phone')."</td><td><table><tr class='m-phone-desc'><td>".t('Country code')."</td><td>".t('City code')."</td><td>".t('Phone number')."</td></tr>".
            "<tr>";
  $form['tccode'] = array(
    '#prefix' => $prefix."<td class='td-value'>+",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => $currCountryPhone,
    '#attributes'=>array('readonly'=>'readonly')
  );
  $form['tcode'] = array(
    '#prefix' => "<td class='td-value'>(",
    '#suffix' => ')</td>',
    '#type' => 'textfield',
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => ($form_state['post']['tcode']? $form_state['post']['tcode']: '')
  );
  $form['tnumber'] = array(
    '#prefix' => "<td class='td-value'>",
    '#suffix' => '</td></tr></table></td></tr>',
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 20,
    '#default_value' => ($form_state['post']['tnumber']? $form_state['post']['tnumber']: '')
  );


  //email
  $form['email'] = array(
    '#prefix' => "<tr id='tr_email'><td class='row-label required'>".t('email').$reqstar . theme('ori_tooltiplink', 3)."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' =>t('email'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#attributes'=>array('class'=>'validate[required] text-input') ,
    '#default_value' => ($form_state['post']['email']? $form_state['post']['email']: '')
  );


  //registration purpose
  $purpose_types = variable_get('constant_purposes', array());
  $purptype_default = ($form_state['post']['purposeType']>=0) ? $form_state['post']['purposeType'] : -1;
  $form['purposeType'] = array(
    '#prefix' => "<tr><td class='row-label required'>".t('Registration purpose').$reqstar."</td><td class='td-value'><div id='purpose-wrapper'>",
    '#suffix' => '</div></td></tr>',
    '#title' => t('Registration purpose'),
    '#type' => 'radios',
    '#options' => $purpose_types,
    '#default_value' => $purptype_default,
  );

  ;
  $form['pother'] = array(
    '#prefix' => "<tr id='tr_pother' ".($purptype_default==3? "":"class='hidden'")."><td class='row-label required'></td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textarea',
    '#description' => t('Опишите вашу цель'),
    '#default_value' => empty($form_state['post']['pother'])? "" : $form_state['post']['pother']
  );

  //additional
  $form['profession'] = array(
    '#prefix' => "<tr><td class='row-label'>".t('Your profession')."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#title' => t('Your profession'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 40,
    '#default_value' => empty($form_state['post']['profession'])? "" : $form_state['post']['profession']
  );


  $form['captcha'] = array(
    '#prefix' => "<tr class='captcha-wrapper' ><td class='row-label required'>".t('Enter image code')."</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'captcha',
    '#captcha_type' => 'image_captcha/Image',
    '#attributes'=>array('class'=>'validate[required] text-input'),
    '#post_render'=> array('_captcha_post_render')
);



  $form['table_end'] = array(
    '#type' => 'markup',
    '#value' => '</table>'
  );


 $referer = isset($form_state['post']['referer_hidden'])? $form_state['post']['referer_hidden'] : ori_regform_get_referer();
 $form['referer_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => (string)$referer
 );


 $referer_uid = isset($form_state['post']['referal_partner_uid_hidden'])? $form_state['post']['referal_partner_uid_hidden'] : (int)$_SESSION['OriRegform']['referal_partner_uid'];
 if ((int)$_GET['ref']>0) $referer_uid = (int)$_GET['ref'];
 $form['referal_partner_uid_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => $referer_uid
 );


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Отправить данные'),
    '#attributes'=>array('class'=>'ori-form-submit')
  );





    //add common
    $setting = array(
                     'def_country' =>$currCountry,
                     'post_index'  =>$form_state['post']['index'],
                     'post_state' =>$form_state['post']['state_hidden'],
                     'curr_cacheIndex' =>$curr_cacheIndex,
                     'baseurl' => variable_get('ori_regform_baseurl','')
                     );

    drupal_add_js($path .'/js/ori_regform.js', 'module');
    drupal_add_js(array('ori_Regform' => $setting), 'setting');
    drupal_add_css($path .'/css/ori_regform.css');


  return $form;
}

function _captcha_post_render($content, $elements){
  return str_replace('class="form-text required"','class="form-text required validate[required] text-input"',$content);
}

/**
 *
 * @param type $form
 * @param type $form_state
 */
function ori_regform_regform_validate($form, &$form_state){

  //regform_validate
  $resp = OriClient::init()->validateUser($form_state['values']);
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
}
/**
 *
 * @param type $form
 * @param type $form_state
 */
function ori_regform_regform_submit($form, &$form_state){

   // add additional information
   $form_state['values']['site_referer'] = isset($_SESSION['OriRegform']['site_referer'])? $_SESSION['OriRegform']['site_referer']: $form_state['values']['hidden_referer'];
   $form_state['values']['http_host'] = $_SERVER['HTTP_HOST'];

   $res = OriClient::init()->registerUser($form_state['values']);
   module_invoke_all('regform_submit_after', $res);

   _regform_submit_after_redirect($res);
}
/**
 *
 * @param type $res
 */
function _regform_submit_after_redirect($res = array()) {
    if (!$res || isset($res['error_message'])){
        watchdog('ori_server', 'After registration redirect error: @err', array('@err' => (string)$res['error_message']), WATCHDOG_ERROR);
    }
    else {
        drupal_goto($res['register_page']);
    }
}






/**
 *  Ajax callback manager
 * @param type $callback_name
 * @return type
 */
function ori_regform_ajax_callback($callback_name = '') {

   $callback_function = __FUNCTION__ . '_'. $callback_name;
   if(function_exists($callback_function))
       echo  $callback_function();
     else
       echo '{error: cant find callback}';
}



/**
 *  Ajax callback for postindex autocomplete fields
 */
function ori_regform_ajax_callback_getselect() {

    // тип поля autocomplete
    $field = $_POST['field'];
    // ключ поиска
    // код страны по которой ищем
    $country = check_plain($_POST['country']);


    if (!empty($field)) {
        $result = OriRegform::init($country)->getSelect($field);

        return drupal_json(
                array('result' => $result,
                      'count' => count($result))
        );
    }
}

/**
 *  Ajax callback for postindex autocomplete fields
 */
function ori_regform_ajax_callback_autocomplete() {
    //return $callback_name;
    //var_dump($callback_name);
    // тип поля autocomplete
    $field = $_POST['field'];
    // код страны по которой ищем
    $country = check_plain($_POST['country']);

    $arg = $_POST;

    if (!empty($field)) {
        $result = OriRegform::init($country)->getSelect($field, $arg);

        return drupal_json(
                array('result' => $result,
                      'count' => count($result))
        );
    }

    }