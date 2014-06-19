<?php

include_once('pms_users.class.inc');

/**
 * Page: baseurl/groupaction/%
 */
function pms_users_groupaction_page($gaction) {
  return pms_users_page();
}

/**
 * Page: Users list
 */
function pms_users_page() {
  $output  = drupal_get_form('pms_users_filter_form');
  $output .= drupal_get_form('pms_users_content_form');

  return $output;
}

/**
 * init get filters
 */
function _pms_users_filter_form_init() {
  $args = arg();
  $filters = $_SESSION['pms_users_filters'];

  //delete all saved filters keep only from get params
  if ($args[3] == 'groupaction') {
    $filters = array();
  }

  $Allfilters = variable_get('pms_users_table_filters', array());
  foreach ($Allfilters as $k) {
    if (isset($_GET[$k])) {
      if (is_scalar($_GET[$k]))
        $filters[$k] = $_GET[$k];
      if (is_array($_GET[$k]))
        $filters[$k] = array_flip($_GET[$k]);
    }
  }
  //save to other modules
  $_SESSION['pms_users_filters'] = $filters;

  return $filters;
}

/**
 * User list filter form
 * @return string
 */
function pms_users_filter_form($form_state) {
  global $user;

  $path = drupal_get_path('module', 'pms_users');

  //load saved filters
  $filters = _pms_users_filter_form_init();

  $GEOIP_COUNTRY = variable_get('pms_users_default_country_id', 1);
  $Country = empty($filters['country']) ? Country::get($GEOIP_COUNTRY) : Country::get($filters['country']);

  module_load_include('inc', 'pms_userform', 'pms_userform.regform');

  _pms_userform_add_js('datapicker', '', $Country->geocode);
  _pms_userform_add_js('autocomplete');


  $form['filters'] = array(
    '#title' => 'Filters',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => true,
  );
  $form['filters']['table_start'] = array(
    '#type' => 'markup',
    '#value' => '<table>',
    '#weight' => 2
  );
  $form['filters']['lastname'] = array(
    '#prefix' => "<tr><td class='row-label'>Фамилия:</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
    '#default_value' => $filters['lastname'],
    '#weight' => 3
  );
  $form['filters']['firstname'] = array(
    '#prefix' => "<tr><td class='row-label'>Имя:</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
    '#default_value' => $filters['firstname'],
    '#weight' => 4
  );
  $form['filters']['middlename'] = array(
    '#prefix' => "<tr><td class='row-label'>Отчество:</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
    '#default_value' => $filters['middlename'],
    '#weight' => 5
  );
  $form['filters']['email'] = array(
    '#prefix' => "<tr><td class='row-label'>Email:</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 40,
    '#default_value' => $filters['email'],
    '#weight' => 6
  );

  $country_options = Country::select_list();
  $country_options[-1] = t('Any');
  $form['filters']['country'] = array(
    '#prefix' => "<tr id='tr_country'><td class='row-label'>" . t('Country') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Country'),
    '#type' => 'select',
    '#options' => $country_options,
    '#default_value' => $Country->id,
    '#required' => TRUE,
    '#weight' => 6.1
  );
  $showfilters['country'] = $country_options[$Country->id];

  if ($Country)
    $state_options = State::init($Country->geocode)->select_list();

  $state_options[-1] = t('Any');
  $default = empty($filters['state']) ? -1 : $filters['state'];

  $form['filters']['state'] = array(
    '#prefix' => "<tr id='tr_state' ><td class='row-label'>Регион</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('State'),
    '#type' => 'select',
    '#options' => $state_options,
    '#default_value' => $default,
    '#weight' => 7
  );
  $showfilters['state'] = $state_options[$default];

  $form['filters']['town'] = array(
    '#prefix' => "<tr id='tr_town' " . $city_class . "><td class='row-label'>" . t('City') . ":</td><td class='row-value' colspan=3>",
    '#suffix' => t('Please enter @str before', array('@str'=> 'регион')).'</td></tr>',
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => $filters['town'],
    '#weight' => 8
  );
  if (empty($filters['town'])){
     $form['filters']['town']['#attributes'] = array('disabled'=>'disabled');
  }
  drupal_add_js("$(document).ready(function (){
                      $('#edit-state').change(function(){
                        $('#edit-town').attr('disabled','');
                        $('#edit-town').val('');
                        $('#edit-town-hidden').val('');
                      });
              });", 'inline');


  $form['filters']['town_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => (int) $filters['town_hidden']
  );

  $form['filters']['regdate_from'] = array(
    '#prefix' => "<tr><td class='row-label'>Дата регистрации от:</td><td class='row-value'>",
    '#suffix' => '</td>',
    '#type' => 'textfield',
    '#default_value' => ($filters['regdate_from'] ? $filters['regdate_from'] : ''), //date("Y-m-d",time())),
    '#weight' => 9
  );
  $form['filters']['regdate_to'] = array(
    '#prefix' => "<td class='row-label'>до:</td><td class='row-value'>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#default_value' => ($filters['regdate_to'] ? $filters['regdate_from'] : ''), //date("Y-m-d",time() + 23*60*60 + 59*60+59 ))
    '#weight' => 10
  );

  //todo: make autocomplete field
  $form['filters']['regnumber'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Registration number') . ":</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
    '#default_value' => $filters['regnumber'],
    '#weight' => 11
  );

  $form['filters']['psid'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Sponsor regnumber') . ":</td><td class='row-value'  colspan=3>",
    // '#suffix' => '</td></tr>',
    '#type' => 'textfield',
    '#max_length' => 40,
    '#size' => 20,
    '#default_value' => $filters['psid'],
    '#weight' => 12
  );

  if ($filters['psid_none'])
    $showfilters['psid'] = t('None');

  $form['filters']['psid_none'] = array(
    '#title' => t('Select consultants without sponsor'),
    '#suffix' => '</td></tr>',
    '#type' => 'checkbox',
    '#default_value' => $filters['psid_none'],
    '#weight' => 13
  );


  //registration purpose
  $purpose_types = variable_get('constant_purposes', array());
  $purpose_types[-1] = t('Any');
  $purptype_default = ($filters['purpose'])? $filters['purpose'] : -1;
  $form['filters']['purpose'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Purpose') . ":</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Registration purpose'),
    '#type' => 'radios',
    '#options' => $purpose_types,
    '#default_value' => $purptype_default,
    '#weight' => 13.1
  );
  $showfilters['purpose'] = $purpose_types[$purptype_default];

  $show_view_options = array(0 => t('Not view instructions'), 1 => t('View instructions'), -1 => t('Any') );
  $show_vew_default = (isset($filters['show_view']) && $filters['show_view'] >= 0)? $filters['show_view'] : -1;
  $form['filters']['show_view'] = array(
    '#prefix' => "<tr><td class='row-label'>" . t('Show users, who view or not view instructions') . ":</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'radios',
    '#default_value' => $show_vew_default,
    '#options' => $show_view_options,
    '#weight' => 13.2
  );
  $showfilters['show_view'] = $show_view_options[(int)$show_vew_default];

  $form['filters']['addfilters'] = array(
    '#prefix' => "<tr><td class='row-label' colspan=4 style='background:#F4F4F4'>",
    '#suffix' => '</td></tr>',
    '#type' => 'markup',
    '#value' => 'Дополнительные фильтры',
    '#weight' => 14
  );

  $status_options = variable_get('pms_users_accountstatus', array());
  $status_options[0] = t('Any');
  $def = $status_options;
  unset($def[PMS_USERS_STATUS_DISABLED]); //delete disabled

  $default = empty($filters['status']) ? array_keys($def) : array_keys($filters['status']);

  //check permissions
  if (!user_access('views view users_disabled'))
    unset($default[PMS_USERS_STATUS_DISABLED]);

    function _tr($v) {
      return t($v);
    }
    $status_options = array_map("_tr", $status_options);

  $form['filters']['status'] = array(
    '#prefix' => "<tr id='tr_status' ><td class='row-label'>" . t('Status') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Status'),
    '#type' => 'select',
    '#options' => $status_options,
    '#multiple' => true,
    '#size' => count($status_options),
    '#default_value' => $default,
    '#weight' => 15
  );

  //id default filter state then hide showfilter
  $diff = array_diff(array_keys($def), $default);
  if (!empty($diff))
    $showfilters['status'] = implode(',', array_intersect_key($status_options, array_flip($default)));
  else
    $showfilters['status'] = t('All');

  $mail_status_options = array('' => t('Any'), 0 => t('Disable'), 1 => t('Enable'));
  $form['filters']['mail_status'] = array(
    '#prefix' => "<tr id='tr_mail_status' ><td class='row-label'>" . t('Mail status') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Mail status'),
    '#type' => 'select',
    '#options' => $mail_status_options,
    '#default_value' => $filters['mail_status'],
    '#weight' => 16
  );
  $showfilters['mail_status'] = $mail_status_options[$filters['mail_status']];

  if (pms_users_og_valid_sponsor()) {
    $groups_options = pms_users_og_groups_select();
    $groups_options[''] = t('Any');
    $form['filters']['groups'] = array(
      '#prefix' => "<tr id='tr_group' ><td class='row-label'>" . t('Groups') . "</td><td class='row-value'  colspan=3>",
      '#suffix' => '</td></tr>',
      '#title' => t('Groups'),
      '#type' => 'select',
      '#options' => $groups_options,
      '#default_value' => $filters['groups'],
      '#weight' => 17
    );
    $showfilters['groups'] = $groups_options[$filters['groups']];
  }

  $role_options = variable_get('pms_users_roles', array());
  $role_options[''] = t('Any');
  $form['filters']['role'] = array(
    '#prefix' => "<tr id='tr_role' ><td class='row-label'>" . t('Roles') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Roles'),
    '#type' => 'select',
    '#options' => $role_options,
    '#default_value' => $filters['role'],
    '#weight' => 18
  );
  $showfilters['role'] = $role_options[$filters['role']];

  $sex_options = array('' => t('Any'), 0 => t('Female'), 1 => t('Male'));
  $form['filters']['sex'] = array(
    '#prefix' => "<tr id='tr_sex' ><td class='row-label'>" . t('Sex') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Sex'),
    '#type' => 'select',
    '#options' => $sex_options,
    '#default_value' => $filters['sex'],
    '#weight' => 19
  );
  $showfilters['sex'] = $sex_options[$filters['sex']];

  $badindex_options = array('' => t('Any'), 0 => t('Yes'), 1 => t('No'));
  $form['filters']['badindex'] = array(
    '#prefix' => "<tr id='tr_badindex' ><td class='row-label'>" . t('Index exists') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#title' => t('Index exists'),
    '#type' => 'select',
    '#options' => $badindex_options,
    '#default_value' => $filters['badindex'],
    '#weight' => 20
  );
  $showfilters['badindex'] = $badindex_options[$filters['badindex']];

  module_load_include('inc', 'ori_partner_site');
  $partner_site_options = _ori_partner_site_select_options();
  $partner_site_options[0] = t('Any');
  $showfilters['partner_site'] = $partner_site_options[$filters['partner_site']];

  $unsubscribed_options = array('' => t('Any'), 0 => t('Only unsubscribed'), 1 => t('Only subscribers'));
  $form['filters']['sponsor_subscribers'] = array(
    '#prefix' => "<tr id='tr_badindex' ><td class='row-label'>" . t('Sponsor subscription status') . "</td><td class='row-value'  colspan=3>",
    '#suffix' => '</td></tr>',
    '#type' => 'select',
    '#options' => $unsubscribed_options,
    '#default_value' => $filters['sponsor_subscribers'],
    '#weight' => 21.1
  );
  $showfilters['sponsor_subscribers'] = $unsubscribed_options[$filters['sponsor_subscribers']];



  $form['filters']['table_end'] = array(
    '#type' => 'markup',
    '#value' => '</table><br>',
    '#weight' => 997
  );

  $form['filters']['filter'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#weight' => 998
  );
  $form['filters']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#weight' => 999
  );
  $form['#validate'] = array();

  //set forms default_values
  $filters = is_array($filters) ? $filters : array();
  $FILTERS = variable_get('pms_users_table_filters', array()); //all list of filters

  //get allowed filters by role
  if (!pms_users_admin_access()) {
    $FILTERS = array();

    if (pms_users_is_operator())
      $mode = 'operator';
    if (user_access('user is partner'))
      $mode = 'partner';

    $FILTERS = variable_get('pms_users_' . $mode . '_allowed_filters', array());
  }

  //chweck only allowed filters
  foreach ($FILTERS as $k => $fname) {
    if ($filters[$fname] != NULL) {
      $form['filters'][$fname]['#default_value'] = $filters[$fname];
    }

    //current filters message
    if (($filters[$fname] != NULL) || $showfilters[$fname]) {
      $filter_title = ($showfilters[$fname] ? $showfilters[$fname] : $filters[$fname]);
      if ($filter_title != t('Any'))
        $filtersmess[] = '[<i>' . t($fname) . '</i>="<b>' . $filter_title . '</b>"]';
    }
  }

  //filter current values
  $form['filters']['#title'] = t('Filters') . (empty($filtersmess) ? '' : ': ' . implode('x', $filtersmess));

  //add common
  $setting = array(
    'def_country' => $Country->geocode,
    'baseurl' => url(variable_get('pms_userform_baseurl', ''))
  );

  drupal_add_js($path . '/js/pms_users.filter.js', 'module');
  drupal_add_js(array('pms_Users_filters' => $setting), 'setting');
  drupal_add_css($path . '/css/pms_users.list.css');

  $form['#submit'][] = 'pms_users_filter_form_submit';
  $form['#action'] = url(variable_get('pms_users_baseurl', ''));

  return $form;
}

/**
 * User list filter form submit
 * @param type $form
 * @param type $form_state
 */
function pms_users_filter_form_submit($form, &$form_state) {
  $filters = $_SESSION['pms_users_filters'];
  $filters = is_array($filters) ? $filters : array();
  $values = $form_state['values'];
  if ($values['op'] == t('Filter')) {

    //save filter form submitted values in session
    $allFilters = variable_get('pms_users_table_filters', array());
    foreach ($allFilters as $filtername) {
      if (isset($values[$filtername])) {
        $filters[$filtername] = $values[$filtername];
      }
      else {
        unset($filters[$filtername]);
      }
    }

    if (isset($values['country'])) {
      if ($values['country'] == -1) {
        unset($filters['country']);
      }
      else {
        $filters['country'] = $values['country'];
      }
    }


    if (isset($values['state'])) {
      if ($values['state'] == -1) {
        unset($filters['state']);
      }
      else {
        $filters['state'] = $values['state'];
      }
    }

    if (isset($values['town']) && isset($values['town_hidden'])) {
      $filters['town_hidden'] = $values['town_hidden'];
    }
    else
      unset($filters['town_hidden']);

    if (isset($values['purpose'])) {
      if ($values['purpose'] == -1) {
        unset($filters['purpose']);
      }
      else {
        $filters['purpose'] = $values['purpose'];
      }
    }
    $filters['psid_none'] = $values['psid_none'];
  }

  if ($values['op'] == t('Reset')) {
    $filters = array();

    //clear selected array if filter button submit
    if ($_SESSION['pms_users_list'] == 'filtered')
      unset($_SESSION['pms_users_list']);
  }

  $_SESSION['pms_users_filters'] = $filters;
}

/**
 * User list form
 * @return string
 */
function pms_users_content_form($form_state) {
  $form = array();

  //allow modules add their group_operations
  $gactions_info = module_invoke_all('group_operations_info', array());
  foreach ($gactions_info as $actionkey => $action) {
    // collect group actions information
    if (user_access('execute ' . $actionkey)) {
      $form_state['storage']['#gactions_options'][$actionkey] = $action['title'];
      $form_state['storage']['#gactions_button'][$actionkey] = $action['button'];
    }
  }

  //hide form if no operations
  if (!empty($form_state['storage']['#gactions_options'])) {
    pms_users_operations_form($form, $form_state); // group operations
    pms_users_operations_select_options($form);    // select options
  }

  $form['table']['#value'] = _pms_users_list_table($form_state);

  return $form;
}

/**
 *
 */
function _pms_users_list_is_selected() {
  return !empty($_SESSION['pms_users_list']);
}

/**
 *
 */
function pms_users_operations_select_options(&$form) {
  $selected = _pms_users_list_is_selected();
  if ($selected) {
    $showselected = is_array($_SESSION['pms_users_list']) ? count($_SESSION['pms_users_list']) : t($_SESSION['pms_users_list']);
  }

  //top buttons
  $form['buttons_top'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select options') . ($selected ? ' (' . $showselected . ')' : ''),
    '#collapsible' => true
  );

  //add selected list
  if ($selected) {
    $form['buttons_top']['selected']['#value'] = _pms_users_list_selected();
    $form['buttons_top']['selected']['#prefix'] = '<div class="group_operations_description" >';
    $form['buttons_top']['selected']['#suffix'] = '</div>';
  }

  //select button if(user_access())
  if (!in_array($_SESSION['pms_users_list'], array('all', 'filtered'))){
    $form['buttons_top']['select'] = array(
      '#type' => 'submit',
      '#value' => t('Add selected')
    );
}
  if (!$selected){
    $form['buttons_top']['selectall'] = array(
      '#type' => 'submit',
      '#value' => t('Select All')
    );
}

  if ($_SESSION['pms_users_filters'] && $_SESSION['pms_users_list']!='filtered') {
    $form['buttons_top']['selectfiltered'] = array(
      '#type' => 'submit',
      '#value' => t('Select filtered')
    );
  }

  //reset button
  if ($selected) {
    $form['buttons_top']['resetselect'] = array(
      '#type' => 'submit',
      '#value' => t('Reset')
    );
  }
}

/**
 * show selected list
 */
function _pms_users_list_selected() {
  $out = '';

  if (_pms_users_list_is_selected()) {
    if (is_array($_SESSION['pms_users_list'])) {
      if (count($_SESSION['pms_users_list']) < 20) {
        $selecteditems = array();
        //query
        $sql = "select concat(firstname,' ',lastname) as title from {ori_user}  where id in (" . implode(',', $_SESSION['pms_users_list']) . ")";

        $q = db_query($sql);
        while ($row = db_fetch_array($q)) {
          $selecteditems[] = $row['title'];
        }
        $out = '<ul class="selected-items"><li>' . implode('</li><li>', $selecteditems) . '</li></ul>';
      }
      else
        $out = count($_SESSION['pms_users_list']) . ' ' . t('items selected');
    }

    if ($_SESSION['pms_users_list'] == 'all')
      $out = t('All');

    if ($_SESSION['pms_users_list'] == 'filtered')
      $out = t('Filtered');
  }

  return $out;
}

/**
 *
 * @return string
 */
function pms_users_operations_form(&$form, $form_state) {
  $args = arg();

  $gaction_fieldset_collapse = !(isset($_SESSION['pms_users_list']) || $args[3] == 'groupaction' || isset($_SESSION['subscription_mail_list']));
  $form['filters'] = array(
    '#title' => t('Group operations'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => $gaction_fieldset_collapse,
    '#attributes' => array('class' => 'groupactions-group')
  );

  $options = $form_state['storage']['#gactions_options'];
  $options[''] = t('Not selected');

  $form['filters']['group_operations'] = array(
    '#title' => t('with selected users'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => ''
  );

  $form['filters']['group_operations_description'] = array(
    '#type' => 'markup',
    '#value' => '',
    '#prefix' => '<div class="group_operations_description">',
    '#suffix' => '</div>'
  );

  $form['filters']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Execute operation'),
  );

  $form['#submit'][] = 'pms_users_operations_form_submit';

  //open group fieldset if page of group operation
  //$form['filters']['#collapsed'] = false;

  $curr_gaction = empty($form_state['values']['group_operations']) ? '' : $form_state['values']['group_operations'];
  $gaction = $curr_gaction ? $curr_gaction : $args[4];
  //set operation
  $form['filters']['group_operations']['#default_value'] = $gaction;

  //set operation comment
  $result = module_invoke_all('group_operations_description');
  if ($result)
    $form['filters']['group_operations_description']['#value'] = implode('', $result);


  drupal_add_js("$(document).ready(function (){
                    //hide all
                    $('.group_operations_description .description').hide();
                    $('.groupactions-group #edit-submit').hide();

                    //show current
                    $('#edit-group-operations').change(function(){
                       var curr = $(this).val();
                       $('.group_operations_description .description').hide();
                       if(curr.length > 0){
                          $('.group_operations_description #'+curr).show();
                          $('.groupactions-group #edit-submit').show();
                         }
                       else
                         $('.groupactions-group #edit-submit').hide(); //hide submit button

                    });


                    //show descr of current gaction
                    var currgaction = '" . (string) $gaction . "';
                    if(currgaction.length>0){
                       $('.group_operations_description #" . $gaction . "').show();
                       $('.groupactions-group #edit-submit').show();
                       }
                    });", 'inline');
  //}
}

/**
 *
 */
function pms_users_operations_form_submit($form, &$form_state) {
  $operation_name = $form['#post']['group_operations'];
  $ids = $form['#post']['id'];
  $selected = $_SESSION['pms_users_list'];
  $groupactions_button_clicked = $form_state['clicked_button']['#id'] == 'edit-submit';

  //save selected
  _pms_users_operations_form_submit_selected($form, $form_state);

  //action button submit
  $gactions_info = module_invoke_all('group_operations_info', array());
  foreach ($gactions_info as $actionkey => $action) {
    $ids = $form['#post']['button_' . $actionkey];
    if (!empty($ids) && $action['button']) {

      $operation_name = $actionkey;
      $selected = array(key($ids));
      $groupactions_button_clicked = true;
    }
  }

  //if group operations submit button clicked
  if ($groupactions_button_clicked) {
    if ($operation_name && !empty($selected)) {

      //get selected uids array
      $uids = (in_array($selected, array('filtered', 'all')))? _pms_users_get_selected_users_query($selected): $selected;

      //allow modules add their group_operations
      $message = module_invoke_all(
          'group_operations_callback', $operation_name, array('selected' => $uids, 'form_state' => $form_state, 'mode'=>$selected)
      );
      //empty selected array
      unset($_SESSION['pms_users_list']);
      unset($uids);

      if ($message) {
        if (is_array($message)) {
          $status = key($message);
          $message = $message[$status];
        }

        drupal_set_message($message, $status);
      }
    }
  }
}

/**
 *
 */
function _pms_users_operations_form_submit_selected($form, $form_state) {
  $ids = $form['#post']['id'];
  $butt = $form_state['clicked_button']['#id'];

  if (in_array($butt, array('edit-select', 'edit-goselectusers')) && is_array($ids)) {
    //save selected
    $oldids = empty($_SESSION['pms_users_list']) ? array() : $_SESSION['pms_users_list'];
    if (is_array($oldids))
      $_SESSION['pms_users_list'] = array_unique(array_merge($oldids, array_keys($ids)));
  }

  //empty selected items
  if ($butt == 'edit-resetselect') {
    //save selected
    unset($_SESSION['pms_users_list']);
  }

  // select all
  if ($butt == 'edit-selectall') {
    $_SESSION['pms_users_list'] = 'all';
  }

  // select filtered
  if ($butt == 'edit-selectfiltered') {
    $_SESSION['pms_users_list'] = 'filtered';
  }
}

/**
 *
 * @return type
 */
function _pms_users_list_table($form_state) {
  $PMS_USERS_STATUS = variable_get('pms_users_accountstatus', array());
  $is_admin = pms_users_admin_access();

  drupal_add_js(drupal_get_path('module', 'pms_users') . '/js/pms_users.list.js');

  $selall_checkbox = array(
    '#type' => 'checkbox',
    '#id' => 'selAllUsers',
    '#name' => 'selAll',
    '#default_value' => ''
  );

  $headers = array(
    array('data' => drupal_render($selall_checkbox)),
    array('data' => t('Country')),
    array('data' => t('Regnumber')),
    array('data' => t('Username'), 'field' => 'u.lastname'),
    array('data' => t('Date registr'), 'field' => 'u.regdate'),
    array('data' => t('Email'), 'field' => 'u.email'),
    //array('data' => t('Error text')), //admin field
    array('data' => t('Status'), 'field' => 'u.status'),
    array('data' => t('Role')),
    array('data' => t('Training statistic')),
    array('data' => t('Instruction newbie')),
  );

  //delete admin fields
  if (!pms_users_admin_access()) {
    unset($headers[6]);
  }

  // build query
  $qresult = _pms_users_list_query_where();
  extract($qresult); //return sql sql_count
  //default sort
  if (empty($_GET['order'])) {
    $_GET['order'] = t('Date registr');
    $_GET['sort'] = 'desc';
  }

  $COUNTRY = Country::select_list();
  $selected = _pms_users_list_is_selected();
  if ($selected)
    $selectedids = $_SESSION['pms_users_list'];

  $sql .= tablesort_sql($headers);
  $limit = 20;


  $result = pager_query($sql, $limit, 0, $sql_count);
  global $pager_total_items;
  $count = $pager_total_items[0];

  $resArr = $ids = $uids = array();
  while ($row = db_fetch_object($result)) {
    $resArr[] = $row;
    $ids[] = $row->id;
    $uids[] = $row->uid;
  }
  $resInfo = module_invoke_all('pms_users_result_alter', $resArr, $ids, $uids);
  //dpm($resInfo);
  /* return result as
   * .. (Array, 2 elements)
    pms_users (Array, 20 elements)
    subscription (Array, 20 elements)
   */

  $data = array();
  $i = 0;
  foreach($resArr as $row){
    $i++;
    $trclass = '';

    $num_checkbox = array(
      '#type' => 'checkbox',
      '#id' => 'id' . $row->id,
      '#name' => 'id[' . $row->id . ']',
      '#attributes' => array('class' => 'idCheckbox'),
      '#default_value' => 0
    );
    //check selected checkbox
    if ($selected) {
      $checked = false;
      if (is_array($selectedids) && in_array($row->id, $selectedids))
        $checked = 'checked';
      if (in_array($selectedids, array('all', 'filtered')))
        $checked = 'checked';

      if ($checked)
        $num_checkbox['#attributes']['checked'] = 'checked';
    }

    //email colorize
    $email = $row->email;
    if ($row->email_confirmed)
      $email = '<b style="color:green;display:block;">' . $row->email . '</b>';
    if (!$row->mail_status)
      $email = '<b style="color:red;display:block;">' . $row->email . '</b>';
    //if user disabled tr colorize
    if ($row->status == PMS_USERS_STATUS_DISABLED) {
      $trclass = 'disabled';
    }

    unset($add_title);
    $add_title.= ($row->psid > 0) ? '[' . t('Consult') . ']' : '';
    $add_title.= ($row->badindex > 0) ? '[<i style="color:red">' . t('Index') . '</i>]' : '';
    $add_title.= ($row->groupname) ? '<br />[' . t($row->groupname) . ']' : '';
    $add_title = ($add_title) ? '<br />' . $add_title : '';
    // Прверка, смотрел ли пользователь инструкцию, и если да, то добавляем ссылке цвет.
    $class_view = $row->instrview? 'green-user':'';
    $mail_error = ($is_admin && $resInfo['subscription'][$row->id])? '<center>['.l('<b class="red-text" title="Смотреть неотправленные письма">ошибка отправки!</b>','admin/oriflame/pms_users/' . $row->id . '/maillist', array('html' => true)).']</center>':'';

    // make row
    $td = array(
      drupal_render($num_checkbox),
      $COUNTRY[$row->cid],
      $row->regnumber,
      l($row->lastname . ' ' . $row->firstname . ' ' . $row->middlename, 'admin/oriflame/pms_users/' . $row->id . '/edit', array('attributes' => array('title' => 'Регномер: ' . $row->regnumber))) . $add_title,
      date('d/m/Y H:i', $row->regdate).'<br />['.$row->regsource.']',
      $email . $mail_error,
      t($PMS_USERS_STATUS[$row->status]),
      t($row->roles),
      l(t('view'), 'training/view/statistic/' . $row->uid, array(
              'attributes' => array(
                  'class' => $class_view,)
                )
      ),
      l(t('study the instructions'), ori_instruction_viewer_get_instruction_url(user_load($row->uid)) , array( 'attributes' => array('target' => '_blank')))
    );

    //delete admin cell
    if (!pms_users_admin_access()) {
      unset($td[7]);
    }

    $oddeven = ($i % 2) ? 'myodd' : 'myeven';
    if (!$trclass)
      unset($trclass);
    $addrow = array('data' => $td, 'class' => implode('', array($trclass, $oddeven)));

    //add row
    $data[] = $addrow;

    //add action buttons row
    $action_buttons = _pms_users_get_action_buttons($form_state, $row);
    //if gaction buttons exist
    if (!empty($action_buttons) || !empty($row->errortext)) {
      $action_buttons = !empty($action_buttons)?implode('', $action_buttons):'';

      $customtext = module_invoke_all('pms_users_customtext_alter', $row);
      $customtext = '<div style="float:left">' . implode(' | ',  array_reverse($customtext)) .'</div>';

      $data[] = array(
        'data' => array('data' => array('data' => $customtext . $action_buttons, 'class' => 'operations', 'colspan' => count($td))),
        'class' => $oddeven
      );
    }
  }

  $out .= theme('table', $headers, $data, array('id' => 'pms_users_table'), 'Найдено записей: ' . $count);
  $out .= theme('pager', NULL, $limit, 0);

  return $out;
}




function pms_users_pms_users_customtext_alter($row){
  if(!empty($row->errortext))
     return '<span class="red-text">'.check_plain($row->errortext).'</span>';
}

/**
 *
 */
function _pms_users_get_action_buttons($form_state, $row) {
  $out = array();
  if (!empty($form_state['storage']['#gactions_button'])) {
    foreach ($form_state['storage']['#gactions_button'] as $actionkey => $action_button) {
      if (!empty($action_button) && user_access('execute ' . $actionkey)) {
        $action_button = is_array($action_button) ? $action_button : array();

        $button = array_merge(
            $action_button, array(
          '#type' => 'button',
          '#value' => '', //$form_state['storage']['#gactions_options'][$actionkey],
          '#name' => 'button_' . $actionkey . '[' . $row->id . ']',
          '#attributes' => array('class' => 'button_' . $actionkey, 'title' => $form_state['storage']['#gactions_options'][$actionkey])
        ));

        //add context data to render functions
        $button['#context'] = (array) $row;
        $out[] = drupal_render($button);
        unset($button['#context']);
      }
    }
  }

  return $out;
}

/**
 * Helper functions
 */


/**
 *
 * @global type $user
 * @param type $addfilters
 * @return type
 */
function _pms_users_list_query_where($addfilters = array()) {
  $filters = $_SESSION['pms_users_filters'];
  $filters = is_array($filters) ? $filters : array();
  //add custom filters
  $filters = array_merge($filters, $addfilters);

  $fields = $where = $join = $having = array();
  $fields[] = 'u.*';



  $f = $filters['email'];
  if ($f != NULL) {
    $where[] = " u.email like \"%%$f%%\" ";
  }
  $f = $filters['firstname'];
  if ($f != NULL) {
    $where[] = " u.firstname like '%$f%' ";
  }
  $f = $filters['lastname'];
  if ($f != NULL) {
    $where[] = " u.lastname like '%$f%' ";
  }
  $f = $filters['middlename'];
  if ($f != NULL) {
    $where[] = " u.middlename like \"%%$f%%\" ";
  }
$f = $filters['regnumber'];
  if ($f != NULL) {
    $where[] = " u.regnumber like \"$f%%\" ";
  }
    $f = $filters['purpose'];
  if ($f != NULL && $f >=0 && $f != 3) {
    $where[] = " u.purid = $f ";
  }




  $GEOIP_COUNTRY = variable_get('pms_users_default_country_id', 1);
  $Country = empty($filters['country']) ? Country::get($GEOIP_COUNTRY) : Country::get($filters['country']);
  if ($filters['country'] > 0)
  $currCountry = Country::get($Country->id);

  $f = $Country->id;
  if ($f != NULL) {
    $where[] = " u.cid = $f ";
  }
  $f = $filters['state'];
  if ($f != NULL) {
    $where[] = " ix.state_id = $f ";
    $join['postindex'] = " LEFT JOIN {ori_" . $currCountry->geocode . "_postindex} AS ix ON u.pindex = ix.pindex";
    $fields[] = " ix.* ";
  }
  $f = $filters['town_hidden'];
  if (isset($f) && $filters['town']) {
    $join['postindex'] = " LEFT JOIN {ori_" . $currCountry->geocode . "_postindex} AS ix ON u.pindex = ix.pindex";
    $where[] = " ix.city_id = $f ";
  }

  // dpm($filters);

  $f = $filters['regdate_from'];
  if ($f != NULL) {
    $where[] = " u.regdate >= " . strtotime(str_replace('/', '-', $f));
  }
  $f = $filters['regdate_to'];
  if ($f != NULL) {
    $where[] = " u.regdate <= " . (intval(strtotime(str_replace('/', '-',$f))) + 23 * 60 * 60 + 59 * 60 + 59);
  }
  $f = $filters['mail_status'];
  if ($f != NULL) {
    $where[] = " ms_su.mail_status = $f ";
    $join[] = " INNER JOIN {ori_subscription_user} AS ms_su ON ms_su.id = u.id";
    $fields[] = " ms_su.mail_status ";
  }
  $f = $filters['role'];
  if ($f != NULL) {
    $where[] = " role_r.rid = $f ";
    $join[] = " INNER JOIN {users_roles} AS role_ur ON role_ur.uid = u.uid";
    $join[] = " INNER JOIN {role} AS role_r ON role_r.rid = role_ur.rid";
    $fields[] = " role_r.rid";
  }


  $f = $filters['groups'];
  if ($f != NULL) {
    $having[] = " LOCATE('(".$f.")',grouprid)>0 ";
   // $where[] = " groups.rid = $f ";
  }

  $join[] = " LEFT JOIN {ori_og_groups2users} AS g2u ON g2u.uid = u.id";
  $join[] = " LEFT JOIN {ori_og_groups} AS groups ON groups.rid = g2u.rid";
  $fields[] = " GROUP_CONCAT(distinct groups.name SEPARATOR ',<br />') as groupname,  GROUP_CONCAT(distinct  CONCAT('(',groups.rid,')')) as grouprid";


  $f = $filters['sex'];
  if ($f != NULL && $f != '') {
    $where[] = " u.sex = " . (int) $f;
  }

  $f = $filters['badindex'];
  if ($f != NULL && $f != '') {
    $where[] = " u.badindex = " . (int) $f;
  }

  $f = $filters['sponsor_subscribers'];
  if (is_numeric($f)) {
    $join[] = " LEFT JOIN {ori_partner_maillist} AS pml ON pml.partner_id = u.uid";
    $where[] = " pml.allow_send_emails = " . (int) $f;
  }

  $f = $filters['status'];
  //set default values
  if ($f == NULL) {
    $default = array_keys(variable_get('pms_users_accountstatus', array()));
    unset($default[0]);
    unset($default[4]);
    $f = $default;
  }

  //check permissions for view users_disabled
  if (!empty($f) && in_array(4, $f) && !user_access('view users_disabled')){
      unset($f[4]);
  }
  if ($f && !(sizeof($f) == 1 && current($f) == 0)) {
    $where[] = " u.status IN (" . implode(',', $f) . ") ";
  }


  //todo make permission for superadmin
  global $user;
  if (pms_users_admin_access()) {
    //checkbox - select consult without sponsors
    if ($filters['psid_none']) {
      $where[] = " u.psid = 0 or  u.psid = -1";
    } else {
      $f = $filters['psid'];
      if ($f != NULL && $f != '') {
        $sp = User::get(array('regnumber' => $f));
        if ($sp) {
          $where[] = " u.psid = " . $sp->uid;
        } else {
          $where[] = " u.id = -1";
          drupal_set_message(t('There is no sponsor with regnumber "@regnumber"', array('@regnumber' => $f)));
        }
      }
    }
  } else {
    //for partner show only his child users
    $account = User::get(array('uid' => $user->uid));
    $where[] = " u.psid = " . $account->uid . ' AND u.id != ' . $account->id;
  }

  //join users roles
  $fields[] = " GROUP_CONCAT(distinct ro.name) AS roles, GROUP_CONCAT(distinct ro.rid) AS rolesid ";
  $join[] = " LEFT JOIN {users_roles} AS ur ON u.uid = ur.uid";
  $join[] = " LEFT JOIN {role} AS ro ON ur.rid = ro.rid";



  //lets modules change sql query
  $rez = module_invoke_all('pms_users_list_query_alter', $fields, $join, $where);
  $fields = _array_push_all($fields, $rez['fields']);
  $join   = _array_push_all($join, $rez['join']);
  $where  = _array_push_all($where, $rez['where']);
  $having  = _array_push_all($having, $rez['having']);

  $sql = "select " . implode(', ', $fields) . " FROM {ori_user} AS u ";
  $sql_count = "select u.id from {ori_user} AS u ";

  //join
  if (!empty($join)) {
    $join_query = implode('', $join);
    $sql.= $join_query;
    $sql_count .= $join_query;
  }

 //where
  if (!empty($where)) {
    $where_query = ' WHERE ' . implode(' AND ', $where);
    $sql .= $where_query;
    $sql_count .= $where_query;
  }

  $sql_count.=' GROUP BY u.id';
  $sql_count = 'select count(tblcnt.id) from (' . $sql_count . ') as tblcnt';
  $sql.=' GROUP BY u.id';
  $sql.= empty($having)? '': ' HAVING '.implode(' AND ', $having);

    //dpm(array('sql'=>$sql, 'sql_count'=>$sql_count));
  return array('sql' => $sql, 'sql_count' => $sql_count);
}


function _array_push_all($arr,$arr2){

  if (!empty($arr2) && is_array($arr2)) {
     foreach($arr2 as $v){
       array_push($arr, $v);
     }
  }

return $arr;
}

/**
 * User edit page
 * @param type $uid
 * @return type
 */
function pms_users_edit_page($uid) {
  if (is_numeric($uid)) {

    $account = User::get($uid);
    //if user is not exists
    if (!$account) {
      return drupal_not_found();
    }

    return drupal_get_form('pms_users_edit_form', $uid);
  }
  else
    drupal_goto(variable_get('pms_users_baseurl', ''));
}

/**
 * User edit form
 * @param type $form_state
 * @return string
 */
function pms_users_edit_form($form_state, $id) {
  global $user;
  $reqstar = '<span class="form-required" title="Обязательное поле">*</span>';
  $account = User::get($id);
  $druaccount = user_load($account->uid);
  drupal_set_title($account->lastname . ' ' . $account->firstname . ' ' . $account->middlename . ' (User ORIFLAME profile)');

  module_load_include('inc', 'pms_userform', 'pms_userform.regform');

  //init form_state vars
  _pms_users_edit_form_prepare($form_state, $account);

  // start table
  $form['table_start'] = array(
    '#type' => 'markup',
    '#value' => "<div id='pms_userform_page_content'><table class='user-table'>",
    '#weight' => 1
  );

  //user level roles
  if (pms_users_admin_access(PMS_USERS_OPERATOR_ROLE) && !in_array('partner_moderation', $druaccount->roles)) {
    $level_roles = _pms_users_get_level_roles();
    $current = array_intersect_key($level_roles, $druaccount->roles);
    $default = (is_array($current) && key($current) > 0) ? key($current) : -1;

    //dpm($current);
    $options = $level_roles;

    $form['user_role'] = array(
      '#prefix' => "<tr id='tr_role'><td class='row-label required'>" . t('Access level') . $reqstar . "</td><td class='td-value'>",
      '#suffix' => '</td></tr>',
      '#type' => 'select',
      '#title' => t('Access level'),
      '#default_value' => $default,
      '#options' => $options,
      '#required' => true,
      '#weight' => 1.1
    );
  }

  //user status
  if (pms_users_admin_access()) {
    $form['status'] = array(
      '#prefix' => "<tr id='tr_status'><td class='row-label required'>" . t('Status') . $reqstar . "</td><td class='td-value'>",
      '#suffix' => '</td></tr>',
      '#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => (isset($form_state['post']['status']) ? $form_state['post']['status'] : ''),
      '#options' => variable_get('pms_users_accountstatus', array()),
      '#required' => true,
      '#weight' => 2
    );
  }

  $form['errortext'] = array(
    '#type' => 'textarea',
    '#prefix' => "<tr><td class='row-label' style='color:red'>" . t('Error text') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#value' => $form_state['post']['errortext'],
    '#attributes' => array('style' => 'color:red;border:1px dotted red'),
    '#rows' => 5,
    '#weight' => 3
  );

  // site_referer
  $form['site_referer'] = array(
    '#type' => 'markup',
    '#prefix' => "<tr><td class='row-label required'>" . t('Site referer') . "</td><td class='td-value'>",
    '#value' => $form_state['post']['site_referer'],
    '#suffix' => '</td>',
    '#weight' => 4
  );

  // regnumber
  $regnumber_type = (pms_users_is_operator() && $form_state['post']['regnumber'])?'markup':'textfield';
  $form['regnumber'] = array(
    '#type' => $regnumber_type,
    '#prefix' => "<tr><td class='row-label required'>" . t('Oriflame regnumber') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#value' => $form_state['post']['regnumber'],
    '#weight' => 5,
     //'#attributes'=>array('disabled'=>'disabled')
  );

  // oriflamepassword
  $form['oriflamepassword'] = array(
    '#type' => 'textfield',
    '#prefix' => "<tr><td class='row-label required'>" . t('Oriflame password') . "</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#value' => $form_state['post']['oriflamepassword'],
    '#weight' => 6
      //'#attributes'=>array('disabled'=>'disabled')
  );

  //if(pms_users_admin_access()){
  $sponsor = User::get(array('uid' => (int) $account->psid));
  if ((int)$form_state['post']['psid']>0 && $sponsor)
    $sponsor_name = l($sponsor->firstname . ' ' . $sponsor->middlename . ' ' . $sponsor->lastname, 'admin/oriflame/pms_users/' . $sponsor->id) . ' (' . $sponsor->regnumber . ')';
  else
    $sponsor_name = '<span class="red">'.(int)$form_state['post']['psid'].' not found!'.'</span>';


  $form['psid'] = array(
    '#type' => 'markup',
    '#prefix' => "<tr><td class='row-label required'>Спонсор</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#value' => '<div style="padding:5px 5px 5px 0px; float:left">' . $sponsor_name . ' </div>',
    '#weight' => 7
  );
  //}
  if (pms_users_admin_access(PMS_USERS_OPERATOR_ROLE)
      || pms_users_is_operator() && $sponsor->uid == variable_get('fake_sponsor_uid',0)) {
    $form['psid']['#suffix'] = '<input type="button" class="form-submit" id="edit-psid-edit-button" value="' . t('Edit') . '"  >';
    $form['psid_edit'] = array(
      '#type' => 'textfield',
      '#suffix' => '<span id="psid_edit_descr" style="display:none;">' . t('Enter new regnumber of sponsor') . '</span></td></tr>',
      '#value' => '',
      '#weight' => 7.5
    );
  }
  else {
    $form['psid']['#suffix']='';
    $form['psid_edit'] ='';
  }


  //include common user edit form
  $baseform = drupal_retrieve_form('pms_userform_regform', $form_state);
  //$baseform = pms_userform_regform($form_state);

  $weight = 10;
  foreach (element_children($baseform) as $key) {
    if (!in_array($key, array('#parameters', '#redirect')))
      $baseform[$key]['#weight'] = $weight++;
    $form[$key] = $baseform[$key];
  }

  $email_confirmed = ($account->email_confirmed) ? '<div style="float:left;color:green">(' . t('confirmed') . ')</div>' : '';
  $form['email']['#suffix'] = $email_confirmed . $form['email']['#suffix'];

  if ($account->regnumber){
  $form['referal_link'] = array(
    '#type' => 'markup',
    '#prefix' => "<tr><td class='row-label'>Реферальная ссылка</td><td class='td-value'>",
    '#suffix' => '</td></tr>',
    '#value' => 'http://'.variable_get('ori_server_regformsite', '') .'/ref/'.$account->regnumber,
    '#weight' => 99
  );
  }


  $form['table_end'] = array(
    '#type' => 'markup',
    '#value' => '</table></div>',
    '#weight' => 100
  );


  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $id
  );
  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => $account->uid
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 999999
  );

  $form['#submit'] = array('pms_users_edit_form_submit');
  $form['#validate'] = array(); //'pms_users_edit_form_validate');
  $form['#redirect'] = variable_get('pms_users_baseurl', '') . '/' . $account->id . '/edit';

  return $form;
}

/**
 * User edit form submit
 * @param type $form
 * @param type $form_state
 */
function pms_users_edit_form_submit($form, &$form_state) {
  //changing account
  $account = User::get(intval(arg(3)), true);
  $values = $form_state['values'];

  //get additional post values
  $values['id'] = $account->id;
  $values['uid'] = $account->uid;
  $values['regnumber'] = $form['#post']['regnumber'];
  $values['psid_edit'] = $form['#post']['psid_edit'];
  $values['oriflamepassword'] = $form['#post']['oriflamepassword'];
  $values['errortext'] = $form['#post']['errortext'];
  $values['user_role'] = $form['#post']['user_role'];

  //get altered account object
  $editedaccount = _pms_users_altered_account($account, $values);

  //alter account by role permissions
  //todo: move to _pms_users_altered_account
  $editedaccount = _pms_users_edituser_role_submit($account, $editedaccount, $values);

  pms_users_update_user($editedaccount, $values);
}

/**
 *
 * @param type $account
 * @param type $values
 * @return stdClass
 */
function _pms_users_altered_account($account, $values) {
  $edit = new stdClass();
  $edit->id = $values['id'];
  $edit->uid = $values['uid'];

  $edit->firstname = mb_strtoupper(mb_substr($values['firstname'], 0, 1)) . mb_substr($values['firstname'], 1);
  $edit->lastname = mb_strtoupper(mb_substr($values['lastname'], 0, 1)) . mb_substr($values['lastname'], 1);
  $edit->middlename = mb_strtoupper(mb_substr($values['middlename'], 0, 1)) . mb_substr($values['middlename'], 1);
  $edit->birthday = strtotime(str_replace('/', '-', $values['birthday']));
  $edit->passportseries = $values['series'];
  $edit->passportnumber = $values['number'];
  $edit->sex = (int) $values['sex'];

  $edit->street = $values['street'];
  $edit->house = $values['house'];
  $edit->houseblock = $values['houseblock'];
  $edit->flat = $values['flat'];

  $edit->tccode = $values['tccode'];
  $edit->tcode = $values['tcode'];
  $edit->tnumber = $values['tnumber'];

  $edit->mccode = $values['mccode'];
  $edit->mprefix = $values['mprefix'];
  $edit->mnumber = $values['mnumber'];

  $edit->email = $values['email'];
 /*
  $edit->purid = $account->purid; //$values['purposeType'];
  if ($edit->purid == 3) {
    $edit->pother = $values['pother'];
  }
  */


  $edit->profession = (string) $values['profession'];


  $edit->regnumber = $values['regnumber'];
  $edit->oriflamepassword = $values['oriflamepassword'];

  $edit->status = $values['status'];
  $edit->errortext = $values['errortext'];

  //let modiules change account before update
  $values['ctype'] = $values['communityType_hidden'];
  drupal_alter('pms_users_change', $edit, $values);

  return $edit;
}

/**
 * Fill form account data
 * @param type $account
 * @return type
 */
function _pms_users_edit_form_prepare(&$form_state, $account) {
  $country = Country::get($account->cid);

  $form_state['post']['country'] = $country->geoname;
  $indexdata = OriGeo::init($country->geocode)->getIndex($account->pindex);
  $dbcity = Town::init($country->geocode)->get($account->tid);

  foreach ($account as $k => $v) {
    $form_state['post'][$k] = $v;
  }

  $form_state['post']['site_referer'] = $account->regsource;
  $form_state['post']['series'] = $account->passportseries;
  $form_state['post']['number'] = $account->passportnumber;
  $form_state['post']['state_hidden'] = $account->stid;
  $form_state['post']['communityType_hidden'] = $account->cmid;
  //$form_state['post']['state_hidden'] = isset($_form_state['post']['state_hidden'])? $_form_state['post']['state_hidden']: $account->stid;
  //$form_state['post']['communityType_hidden'] = $_form_state['post']['communityType_hidden'];
  $form_state['post']['town'] = ($account->tidvalue && $account->tidvalue != $dbcity) ? $account->tidvalue : $dbcity;
  $form_state['post']['town_hidden'] = $account->tid;
  $form_state['post']['district'] = $account->district;
  $form_state['post']['loctitle'] = $account->loctitle;
  //$form_state['post']['town'] = $_form_state['post']['town'];
  //$form_state['post']['town_hidden'] = $_form_state['post']['town_hidden'];

  $form_state['post']['purposeType'] = $account->purid;
  $form_state['post']['status'] = $account->status;

  return $form_state;
}

/**
 * User edit page
 * @param type $uid
 * @return type
 */
function pms_users_drupalprofile_page($user) {
  $account = User::get(array('uid' => $user->uid));
  drupal_set_title($account->lastname . ' ' . $account->firstname . ' ' . $account->middlename . ' (Drupal profile)');

  module_load_include('inc', 'user', 'user.pages');

  return drupal_get_form('user_profile_form', $user, $category = 'account');
}

/**
 * User edit page
 * @param type $uid
 * @return type
 */
function pms_users_oldprofile_page($id) {
  $headers = array('#', 'Имя поля', 'Значение');
  $rows = array('нет данных', '');

  $account = User::get($id);
  drupal_set_title($account->lastname . ' ' . $account->firstname . ' ' . $account->middlename . ' (User old profile)');

  if ($account) {
    $result = db_fetch_array(db_query("select * from {all_reg_users} where mark = %d LIMIT 1", $account->id));
    $rows = array();
    foreach ($result as $k => $v) {
      $rows[] = array(
        $i++,
        $k,
        '<b style="font-weight:bold">' . $v . '</b>'
      );
    }
  }

  return theme('table', $headers, $rows, '');
}