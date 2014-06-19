<?php

//form for both options pages
function subscription_main_options_form($form_state, $type) {

  $form['vars'] = array(
    '#title' => t('Subscription settings'),
    '#type'  => 'fieldset',
  );

  if ($type == 'common') {

    $form['vars']['settings_common'] = array(
      '#title' => t('Settings'),
      '#type'  => 'fieldset',
    );

    //$form['vars'][REGISTER_SUCCESS] = s_create_text_area_for_options(REGISTER_SUCCESS,
    //   'Текст страницы об успешной регистрации');

    $form['vars']['settings_common'][DEACTIVATE_MAILS] = s_create_text_field_for_options(
      DEACTIVATE_MAILS,
      'Текст ссылки на отключение от писем с сайта'
    );

    $form['vars']['settings_common'][ACTIVATE_MAILS] = s_create_text_field_for_options(
      ACTIVATE_MAILS,
      'Текст ссылки на подключение к письмам с сайта'
    );

    $form['vars']['settings_common'][MAILER_FROM] = s_create_text_field_for_options(
      MAILER_FROM,
      'Email с которого отправляются письма.',
      'mailer@oriflame-vip.ru'
    );
    $form['vars']['settings_common'][MAILER_FROM_NAME] = s_create_text_field_for_options(
      MAILER_FROM_NAME,
      'Имя от которого отправляются письма.',
      'Правда-Здорово'
    );

    $form['vars']['settings_common']['subscription_cron_send_limit'] = s_create_text_field_for_options(
      'subscription_cron_send_limit',
      'Кол-во писем отправляемых за 1 запуск крона',
      100
    );

    $form['vars']['activate_mail_page'] = array(
      '#title'       => 'Письмо/Страница: Подключении ко всем рассылкам',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['activate_mail_page'][ACTIVATE_MAILS_PAGE_SUBJECT] = s_create_text_field_for_options(
      ACTIVATE_MAILS_PAGE_SUBJECT,
      'Тема письма о подключении всех рассылок'
    );
    $form['vars']['activate_mail_page'][ACTIVATE_MAILS_PAGE] = s_create_text_area_for_options(
      ACTIVATE_MAILS_PAGE,
      'Текст письма\страницы о включении писем с сайта'
    );

    $form['vars']['deactivate_mail_page'] = array(
      '#title'       => 'Письмо/Страница: Отключение от всех рассылок',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );
    $form['vars']['deactivate_mail_page'][DEACTIVATE_MAILS_PAGE_SUBJECT] = s_create_text_field_for_options(
      DEACTIVATE_MAILS_PAGE_SUBJECT,
      'Тема письма об отключении всех рассылок'
    );
    $form['vars']['deactivate_mail_page'][DEACTIVATE_MAILS_PAGE] = s_create_text_area_for_options(
      DEACTIVATE_MAILS_PAGE,
      'Текст письма\страницы об отключении всех рассылок'
    );
  }


  //subcriptions settings
  if ($type == 'subscription') {

    $form['vars']['settings_subscription'] = array(
      '#title' => t('Settings'),
      '#type'  => 'fieldset',
    );

    $form['vars']['settings_subscription'][SUBSCRIBE_TEXT] = s_create_text_field_for_options(
      SUBSCRIBE_TEXT,
      'Текст ссылки на подписку к рассылке(общий)'
    );

    $form['vars']['settings_subscription'][SUBSCRIBE_TO_ALL] = s_create_text_field_for_options(
      SUBSCRIBE_TO_ALL,
      'Текст ссылки на подписку к рассылкам'
    );

    $form['vars']['settings_subscription'][ALREADY_ACTIVE_OR_APPROVE] = s_create_text_field_for_options(
      ALREADY_ACTIVE_OR_APPROVE,
      'Текст сообщения о том, что человек уже подписан или на подтвержении',
      "Вы уже подключены или на подтверждении подключения к рассылке(кам)"
    );

    $form['vars']['settings_subscription'][UNSUBSCRIBE_TEXT] = s_create_text_field_for_options(
      UNSUBSCRIBE_TEXT,
      'Текст ссылки на отписку от рассылки(общий)'
    );

    $form['vars']['settings_subscription'][REPEATREGISTR_LINK] = s_create_text_field_for_options(
      REPEATREGISTR_LINK,
      'Текст ссылки для повторной регистрации'
    );

    $form['vars']['settings_subscription'][CONFIRM_LINK] = s_create_text_field_for_options(
      CONFIRM_LINK,
      'Текст ссылки подтверждения регистрации'
    );

    $form['vars']['settings_subscription'][INSTRUCTION_LINK] = s_create_text_field_for_options(
      INSTRUCTION_LINK,
      'Текст ссылки просмотра инструкции'
    );

    $form['vars']['settings_subscription'][INSTRUCTION_MINI_LINK] = s_create_text_field_for_options(
        INSTRUCTION_MINI_LINK,
        'Текст ссылки просмотра краткой инструкции'
    );

    $form['vars']['approve'] = array(
      '#title'       => 'Статус: Подтверждение',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['approve'][SUBSCRIPTION_APPROVE_SUBJECT] = s_create_text_field_for_options(
      SUBSCRIPTION_APPROVE_SUBJECT,
      'Тема письма с сообщением о смене статуса рассылки'
    );

    $form['vars']['approve'][SUBSCRIPTION_APPROVE_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_APPROVE_TEXT,
      'Текст письма\страницы о смене статуса рассылки'
    );

    $form['vars']['activate'] = array(
      '#title'       => 'Статус: Активирована',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['activate'][SUBSCRIPTION_ACTIVATE_SUBJECT] = s_create_text_field_for_options(
      SUBSCRIPTION_ACTIVATE_SUBJECT,
      'Тема письма с сообщением о смене статуса рассылки'
    );

    $form['vars']['activate'][SUBSCRIPTION_ACTIVATE_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_ACTIVATE_TEXT,
      'Текст письма\страницы о смене статуса рассылки'
    );

    $form['vars']['deactivate'] = array(
      '#title'       => 'Статус: Отключена',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['deactivate'][SUBSCRIPTION_DEACTIVATE_SUBJECT] = s_create_text_field_for_options(
      SUBSCRIPTION_DEACTIVATE_SUBJECT,
      'Тема письма с сообщением о смене статуса рассылки'
    );

    $form['vars']['deactivate'][SUBSCRIPTION_DEACTIVATE_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_DEACTIVATE_TEXT,
      'Текст письма\страницы о смене статуса рассылки'
    );

    $form['vars']['deactivate_admin'] = array(
      '#title'       => 'Статус: Отключена администратором',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['deactivate_admin'][SUBSCRIPTION_DEACTIVATE_ADMIN_SUBJECT] = s_create_text_field_for_options(
      SUBSCRIPTION_DEACTIVATE_ADMIN_SUBJECT,
      'Тема письма с сообщением о смене статуса рассылки'
    );

    $form['vars']['deactivate_admin'][SUBSCRIPTION_DEACTIVATE_ADMIN_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_DEACTIVATE_ADMIN_TEXT,
      'Текст письма\страницы о смене статуса рассылки'
    );

    $form['vars']['declined'] = array(
      '#title'       => 'Статус: Отказано в подключении',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );

    $form['vars']['declined'][SUBSCRIPTION_DECLINED_SUBJECT] = s_create_text_field_for_options(
      SUBSCRIPTION_DECLINED_SUBJECT,
      'Тема письма с сообщением о смене статуса рассылки'
    );

    $form['vars']['declined'][SUBSCRIPTION_DECLINED_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_DECLINED_TEXT,
      'Текст письма\страницы о смене статуса рассылки'
    );

    $form['vars']['repeatregistr'] = array(
      '#title'       => 'Статус: Повторная регистрация',
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE
    );
    $form['vars']['repeatregistr'][SUBSCRIPTION_REPEATREGISTR_TEXT] = s_create_text_area_for_options(
      SUBSCRIPTION_REPEATREGISTR_TEXT,
      'Текст страницы повторной регистрации'
    );
  }


  $form['type'] = array(
    '#type'  => 'hidden',
    '#value' => $type
  );

  $form['template']['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Сохранить значения'),
  );

  return $form;
}

function subscription_main_options_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  if ($values['type'] == 'common') {
    $vars = array(
      MAILER_FROM,
      MAILER_FROM_NAME,
      REGISTER_SUCCESS,
      DEACTIVATE_MAILS,
      ACTIVATE_MAILS,
      ACTIVATE_MAILS_PAGE,
      ACTIVATE_MAILS_PAGE_SUBJECT,
      DEACTIVATE_MAILS_PAGE,
      DEACTIVATE_MAILS_PAGE_SUBJECT,
      'subscription_cron_send_limit'
    );
  }
  else {
    $vars = array(
      SUBSCRIBE_TEXT,
      SUBSCRIBE_TO_ALL,
      UNSUBSCRIBE_TEXT,
      ALREADY_ACTIVE_OR_APPROVE,
      SUBSCRIPTION_APPROVE_SUBJECT,
      SUBSCRIPTION_APPROVE_TEXT,
      SUBSCRIPTION_ACTIVATE_SUBJECT,
      SUBSCRIPTION_ACTIVATE_TEXT,
      SUBSCRIPTION_DEACTIVATE_SUBJECT,
      SUBSCRIPTION_DEACTIVATE_TEXT,
      SUBSCRIPTION_DEACTIVATE_ADMIN_SUBJECT,
      SUBSCRIPTION_DEACTIVATE_ADMIN_TEXT,
      SUBSCRIPTION_DECLINED_SUBJECT,
      SUBSCRIPTION_DECLINED_TEXT,
      SUBSCRIPTION_REPEATREGISTR_TEXT,
      REPEATREGISTR_LINK,
      CONFIRM_LINK,
      INSTRUCTION_LINK,
      INSTRUCTION_MINI_LINK
    );
  }
  foreach ($vars as $var) {
    $value = $values[$var];
    if ($var == SUBSCRIBE_TEXT) {
      $prev_value = variable_get(SUBSCRIBE_TEXT, '');
      db_query("update {subscription} set smsg = '$value' where smsg = '$prev_value'");
    }
    if ($var == UNSUBSCRIBE_TEXT) {
      $prev_value = variable_get(UNSUBSCRIBE_TEXT, '');
      db_query("update {subscription} set umsg = '$value' where umsg = '$prev_value'");
    }

    variable_set($var, $value);
  }
}

/*
 * Creates drupal text area
 * @var - variable name
 * @title - label text
 * @default_value - value if variable doesn't exists
 */
function s_create_text_area_for_options($var, $title, $default_value = '') {
  $field = array();
  $field[$var] = array(
    '#title'         => $title,
    '#type'          => 'textarea',
    '#default_value' => variable_get($var, $default_value),
  );
  //[$var . 'texteditor']
  $field['format'] = filter_form(FILTER_HTML_STRIP, NULL, array($var . '_format')); // filtered HTML format

  return $field;
}

/*
 * Creates drupal text field
 * @var - variable name
 * @title - label text
 * @required - is required?
 */
function s_create_text_field_for_options($var, $title, $default = '', $required = TRUE) {
  return array(
    '#title'         => $title,
    '#type'          => 'textfield',
    '#size'          => 100,
    '#maxlength'     => 255,
    '#default_value' => variable_get($var, $default),
    '#required'      => $required,
    '#description'   => $default
  );
}

