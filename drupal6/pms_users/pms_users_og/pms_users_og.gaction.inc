<?php

/**
 * Implementation of hook_group_operations_info().
 */
function pms_users_og_group_operations_info($options) {
  global $user;

  $account = User::get(array('uid' => $user->uid));
  $accgroup = pms_users_og_user_groups($account->id);
  $is_not_autoregsponsor = pms_users_is_sponsor() && empty($accgroup[PMS_USERS_OG_AUTOREGSPONSOR]);

  if (user_access('execute role_add')) {
    if (!$is_not_autoregsponsor) {
      $info['role_add'] = array(
        'title' => t('Assign group') . '(Ori groups)',
      );
    }
  }

  if (user_access('execute role_remove')) {
    if (!$is_not_autoregsponsor) {
      $info['role_remove'] = array(
        'title' => t('Remove from group') . '(Ori groups)',
      );
    }
  }

  return $info;
}

/**
 * Implementation of hook_group_operations_callback().
 */
function pms_users_og_group_operations_callback($op, $args = array()) {
  global $user;

  switch ($op) {
    case 'role_add':
    case 'role_remove':
      if (user_access('execute role_add') || user_access('execute role_remove')) {
        $rids = $args['form_state']['clicked_button']['#post']['group_operations_origroups'];
        if ($rids && is_array($args['selected'])) {
          foreach ($args['selected'] as $uid) {

            if ($op == 'role_add') {
              foreach ($rids as $rid => $val) {

                pms_users_og_member_add($rid, $uid);
                $message = t('Groups added to selected users!');
              }
            }

            if ($op == 'role_remove') {
              foreach ($rids as $rid => $val) {
                pms_users_og_member_remove($rid, $uid);
              }
              $message = t('Groups removed to selected users!');
            }
          }
        }
      }
      break;
  }

  return $message;
}

/**
 * Implementation of hook_group_operations_description().
 */
function pms_users_og_group_operations_description() {
  global $user;

  if (user_access('execute role_add') || user_access('execute role_remove')) {
    $groups = pms_users_og_groups_select();

    if (!empty($groups)) {
      $groups_elemadd = $groups_elemdel = '';
      foreach ($groups as $rid => $name) {

        $groupadd = array(
          '#name' => 'group_operations_origroups[' . $rid . ']',
          '#id' => 'group_operations_origroupsadd[' . $rid . ']',
          '#type' => 'checkbox',
          '#title' => $name,
        );
        $groupdel = array(
          '#name' => 'group_operations_origroups[' . $rid . ']',
          '#id' => 'group_operations_origroupsdel[' . $rid . ']',
          '#type' => 'checkbox',
          '#title' => $name,
        );

        $curr = $_SESSION['pms_users_filters']['groups'] == $rid;
        if ($curr) {
          $groupadd['#attributes'] = array('checked' => 'checked');
          $groupdel['#attributes'] = array('checked' => 'checked');
        }

        $groups_elemadd.= drupal_render($groupadd);
        $groups_elemdel.= drupal_render($groupdel);
      }

      $out.= '<div id="role_add" class="description">' . $groups_elemadd . '</div>';
      $out.= '<div id="role_remove" class="description">' . $groups_elemdel . '</div>';
    }
  }

  return $out;
}