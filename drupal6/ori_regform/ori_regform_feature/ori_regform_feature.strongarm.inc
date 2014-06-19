<?php

/**
 * Implementation of hook_strongarm().
 */
function ori_regform_feature_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_agreement';
  $strongarm->value = 'Нажимая "Отправить данные" Вы соглашаетесь на обработку Ваших персональных данных в соответствии с требованиями Федерального закона от 27.07.2006 №152-ФЗ «О персональных данных».';
  $export['ori_regform_agreement'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_copyright';
  $strongarm->value = 'copyright';
  $export['ori_regform_copyright'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_email';
  $strongarm->value = 'admin@mail.ru';
  $export['ori_regform_email'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_group1';
  $strongarm->value = 'important';
  $export['ori_regform_group1'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_group2';
  $strongarm->value = 'important';
  $export['ori_regform_group2'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_group3';
  $strongarm->value = '0';
  $export['ori_regform_group3'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_group4';
  $strongarm->value = '0';
  $export['ori_regform_group4'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_group5';
  $strongarm->value = '0';
  $export['ori_regform_group5'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_settings_list';
  $strongarm->value = array(
    'ori_regform_step1' => 'ori_regform_step1',
    'ori_regform_step2' => 'ori_regform_step2',
    'ori_regform_step3' => 'ori_regform_step3',
    'ori_regform_agreement' => 'ori_regform_agreement',
    'ori_regform_copyright' => 'ori_regform_copyright',
  );
  $export['ori_regform_settings_list'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_step1';
  $strongarm->value = 'Заполните анкетные данные';
  $export['ori_regform_step1'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_step2';
  $strongarm->value = 'Нажмите внизу экрана кнопку «Отправить данные»';
  $export['ori_regform_step2'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_step3';
  $strongarm->value = 'Через 1-2 минуты зайдите на свой e-mail и подтвердите регистрацию перейдя по ссылке в письме';
  $export['ori_regform_step3'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ori_regform_welcome_message';
  $strongarm->value = '';
  $export['ori_regform_welcome_message'] = $strongarm;

  return $export;
}
