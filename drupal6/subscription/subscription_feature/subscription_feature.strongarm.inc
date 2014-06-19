<?php

/**
 * Implementation of hook_strongarm().
 */
function subscription_feature_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'confirm_link';
  $strongarm->value = 'подтвердить регистрацию.';
  $export['confirm_link'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscribe_text';
  $strongarm->value = 'Нажмите сюда, что бы подписаться на';
  $export['subscribe_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'unsubscribe_text';
  $strongarm->value = 'Нажмите сюда, что бы отписаться от';
  $export['unsubscribe_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_userlist_page';
  $strongarm->value = 'admin/oriflame/pms_users';
  $export['subscription_userlist_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_baseurl';
  $strongarm->value = 'admin/oriflame/subscription';
  $export['subscription_baseurl'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_register_event';
  $strongarm->value = 1;
  $export['subscription_register_event'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_birthday_event';
  $strongarm->value = 2;
  $export['subscription_birthday_event'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_confirm_event';
  $strongarm->value = 3;
  $export['subscription_confirm_event'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'activate_mails';
  $strongarm->value = 'Подключиться к письмам';
  $export['activate_mails'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'activate_mails_page';
  $strongarm->value = '<p>Вы успешно подключены ко всем рассылкам сайта www.oriflame-vip.ru</p><p><img alt="картинка4" src="/sites/default/files/u1/papka2/kartinka4.jpg" style="border:0px solid;margin:0px;"></p>';
  $export['activate_mails_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'already_active_or_approve';
  $strongarm->value = 'Вы уже подключены или на подтверждении подключения к рассылке(кам)';
  $export['already_active_or_approve'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'deactivate_mails';
  $strongarm->value = 'Отключиться от писем';
  $export['deactivate_mails'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'deactivate_mails_page';
  $strongarm->value = '<p>Вы успешно отключены от всех рассылок сайта www.oriflame-vip.ru</p><p>Чтобы подключиться вновь нажмите сюда: {{письма|п}}</p><p><img alt="картинка4" src="/sites/default/files/u1/papka2/kartinka4.jpg" style="border:0px solid;margin:0px;"></p>';
  $export['deactivate_mails_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'mailer_from';
  $strongarm->value = 'info@pravda-zdorovo.ru';
  $export['mailer_from'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'register_success';
  $strongarm->value = '';
  $export['register_success'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'activate_mails_page_subject';
  $strongarm->value = 'Вы успешно подключены ко всем рассылкам сайта www.oriflame-vip.ru';
  $export['activate_mails_page_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'deactivate_mails_page_subject';
  $strongarm->value = 'Вы успешно отключены от всех рассылок сайта www.oriflame-vip.ru';
  $export['deactivate_mails_page_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscribe_to_all';
  $strongarm->value = 'Нажмите сюда, что бы подписаться сразу на все рассылки предложенные в этом письме.';
  $export['subscribe_to_all'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_activate_subject';
  $strongarm->value = 'Вы подписаны на рассылку';
  $export['subscription_activate_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_activate_text';
  $strongarm->value = '<p>Поздравляем вы подписаны на рассылку {{рассылки}}.</p>';
  $export['subscription_activate_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_approve_subject';
  $strongarm->value = 'Ваш запрос на подключение к рассылке получен';
  $export['subscription_approve_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_approve_text';
  $strongarm->value = '<p>Запрос на подключение к&nbsp; рассылке {{рассылки}} получен. Ожидайте нашего решения!</p>';
  $export['subscription_approve_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_deactivate_admin_subject';
  $strongarm->value = 'Нам жаль что вы получили часть рассылок сайта www.oriflame-vip.ru';
  $export['subscription_deactivate_admin_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_deactivate_admin_text';
  $strongarm->value = '<p>Мы обнаружили что вы шпион - досвидания.</p>';
  $export['subscription_deactivate_admin_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_deactivate_subject';
  $strongarm->value = 'Ваш запрос на отключение от рассылки выполнен';
  $export['subscription_deactivate_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_deactivate_text';
  $strongarm->value = '<p>Вы отключены от рассылки {{рассылки}}.</p>';
  $export['subscription_deactivate_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_declined_subject';
  $strongarm->value = 'Извините, но вам отказно в подключении к рассылкам';
  $export['subscription_declined_subject'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_declined_text';
  $strongarm->value = '<p>Вы не являетесь участником нашей группы поэтому подключить вас к рассылке {{рассылки}} невозможно.</p>';
  $export['subscription_declined_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_settings_list';
  $strongarm->value = array(
    'subscription_Наше_звание' => 'subscription_Наше_звание',
  );
  $export['subscription_settings_list'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_Наше_звание';
  $strongarm->value = 'Старшие-бриллиантовые директора Oriflame Любовь и Сергей Смолинские.';
  $export['subscription_Наше_звание'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'subscription_repeatregistr_text';
  $strongarm->value = '<p>Ваша заявка на повтрную регистрацию принята</p>';
  $export['subscription_repeatregistr_text'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'repeatregistr_link';
  $strongarm->value = 'Нажми сюда и мы зарегаем тебя повторно';
  $export['repeatregistr_link'] = $strongarm;

  return $export;
}
