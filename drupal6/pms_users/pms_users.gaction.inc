<?php

/**
 * Implementation of hook_group_operations_info().
 */
function pms_users_group_operations_info($options) {
  $info = array(
    'users_remove' => array(
      'title' => t('Delete users'),
      'button' => array('#title' => t('Delete users'))
    ),
    'users_delete' => array(
      'title' => t('Disable users'),
      'button' => array('#title' => t('Disable users'), '#post_render' => array('_gaction_button_user_disable_post_render'))
    ),
    'users_enable' => array(
      'title' => t('Enable users'),
      'button' => array('#title' => t('Enable users'), '#post_render' => array('_gaction_button_user_enable_post_render'))
    ),
    'user_sendpassword' => array(
      'title' => t('Send user password'),
      'button' => array('#title' => t('Send user password'), '#post_render' => array('_gaction_button_user_sendpassword_post_render'))
    )
  );

  return $info;
}

function _gaction_button_user_disable_post_render($content, $elements) {
  //show button only if udser in moderation status
  if ($elements['#context']['status'] == 4)
    $content = '';

  return $content;
}

function _gaction_button_user_enable_post_render($content, $elements) {
  //show button only if user in moderation status
  if ($elements['#context']['status'] != 4)
    $content = '';

  return $content;
}

function _gaction_button_user_sendpassword_post_render($content, $elements) {
  //show button only if udser in moderation status
  $rolesid = explode(',', $elements['#context']['rolesid']);
  if (!$rolesid || !in_array(6, $rolesid))
    $content = '';

  return $content;
}

/**
 * Implementation of hook_group_operations_callback().
 */
function pms_users_group_operations_callback($op, $args = array()) {
  global $user;

  switch ($op) {

    case 'users_remove':
      if (user_access('execute users_remove')) {

        if (is_array($args['selected'])) {
          foreach ($args['selected'] as $uid) {
            User::deleteUser($uid);
          }
        }

        $message = t('Selected users ' . count($args['selected']) . ' are deleted!');
      }
      break;
    case 'users_delete':
      if (user_access('execute users_delete')) {
        if (is_array($args['selected'])) {
          foreach ($args['selected'] as $uid) {
            //User::deleteUserAll($args['selected']);
            User::disable($uid);
          }
          $message = t('Selected users are disabled!');
        }
      }
      break;

    case 'users_enable':
      if (user_access('execute users_enable')) {
        if (is_array($args['selected'])) {
          foreach ($args['selected'] as $uid) {
            User::enable($uid);
          }

          $message = t('Selected users are enable!');
        }
      }
      break;

    case 'user_sendpassword':
      if (user_access('execute user_sendpassword')) {
        $force = true;
        if (count($args['selected']) > variable_get('subscription_cron_send_limit',300)){
          drupal_set_message('Выбрано слишком большое кол-во пользователей, операция выполнится в фоновом режиме');
          $force = false;
        }


        //add activate sending mails
        if (!empty($args['selected'])) {

          module_load_include('inc', 'subscription', 'inc/subscription_users/subscription.users');
          foreach ($args['selected'] as $id) {
            //send all messages by register event
            subscription_user_send_event_mails($id, variable_get('subscription_sendpassword_event', 4), $force);
            //set status confirmed

            $account = User::get($id);
            //delete partner_moderation role
            @db_query('DELETE FROM {users_roles} WHERE uid = %d and rid = %d', $account->uid, PMS_USERS_MODERATION_ROLE);
            //set faster role = partner
            @db_query('INSERT INTO {users_roles} SET uid = %d, rid = %d', $account->uid, PMS_USERS_PARTNER_ROLE);
          }
        }

        $message = t('User passwords has send (' . count($args['selected']) . ')');
      }
      break;
  }


  return $message;
}


/**
 * Implementation of hook_group_operations_description().
 */
function pms_users_group_operations_description() {
  global $user;

  if (user_access('execute users_remove')) {
    $out.= '<div id="users_remove" class="description">Attention selected users would be removed from database</div>';
  }


  return $out;
}