<?php

function ruswebs_server_mail($key, &$message, $params) {
  global $user;

  // Each message is associated with a language, which may or may not be the
  // current user's selected language, depending on the type of e-mail being
  // sent. This $options array is used later in the t() calls for subject
  // and body to ensure the proper translation takes effect.
  $options = array(
    'langcode' => $message['language']->language,
  );

  switch ($key) {
    // Send a simple message from the contact form.
    case 'ruswebs_server_feedback_message':
      $message['subject'] = t('Feedback from [@site-name]', array('@site-name' => $params['site_name']), $options);
      // Note that the message body is an array, not a string.
      //$message['body'][] = t('@name sent you the following message:', array('@name' => $user->name), $options);
      // Because this is just user-entered text, we do not need to translate it.
      // Since user-entered text may have unintentional HTML entities in it like
      // '<' or '>', we need to make sure these entities are properly escaped,
      // as the body will later be transformed from HTML to text, meaning
      // that a normal use of '<' will result in truncation of the message.
      $message['body'][] = check_plain($params['message']);
      $message['body'][] = '------------------------------';
      $message['body'][] = 'Url:'. $params['url'];
      $message['body'][] = 'Time:'. date('d.m.Y H:i:s',$params['timestamp']);
      $message['body'][] = 'From web-site:'. $params['site_name'];
      break;
  }
}

/**
 * Sends an e-mail.
 *
 * @param array $form_values
 *   An array of values from the contact form fields that were submitted.
 *   There are just two relevant items: $form_values['email'] and
 *   $form_values['message'].
 */
function ruswebs_server_mail_send($params) {

  $module = 'ruswebs_server';
  $key = 'ruswebs_server_feedback_message';
  $to = variable_get('ruswebs_server_admin_email', RUSWEBS_SERVER_FEEDBACK_EMAIL);
  $from = $to; //todo:set correct email
  $language = language_default();

  // Send the mail
  $result = drupal_mail($module, $key, $to, $language, $params, $from, TRUE);
  if ($result['result'] == TRUE) {
    $msg = t('Your message has been sent.');
  } else {
    $msg = t('There was a problem sending your message and it was not sent.');
    watchdog('ruswebs_server', __FUNCTION__ . ':'.$msg,'', WATCHDOG_ERROR);
  }

  return array('result' => $result['result'], 'msg' => $msg);
}
