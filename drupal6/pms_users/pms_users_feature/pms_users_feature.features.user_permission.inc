<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function pms_users_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: add ori users
  $permissions['add ori users'] = array(
    'name' => 'add ori users',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: administer ori_partner
  $permissions['administer ori_partner'] = array(
    'name' => 'administer ori_partner',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: administer pms_userform
  $permissions['administer pms_userform'] = array(
    'name' => 'administer pms_userform',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: delete ori users
  $permissions['delete ori users'] = array(
    'name' => 'delete ori users',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute role_add
  $permissions['execute role_add'] = array(
    'name' => 'execute role_add',
    'roles' => array(
      '0' => 'admin',
      '1' => 'director',
      '2' => 'leader',
      '3' => 'partner',
    ),
  );

  // Exported permission: execute role_remove
  $permissions['execute role_remove'] = array(
    'name' => 'execute role_remove',
    'roles' => array(
      '0' => 'admin',
      '1' => 'director',
      '2' => 'leader',
      '3' => 'partner',
    ),
  );

  // Exported permission: execute user_sendpassword
  $permissions['execute user_sendpassword'] = array(
    'name' => 'execute user_sendpassword',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute users_delete
  $permissions['execute users_delete'] = array(
    'name' => 'execute users_delete',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute users_enable
  $permissions['execute users_enable'] = array(
    'name' => 'execute users_enable',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute users_remove
  $permissions['execute users_remove'] = array(
    'name' => 'execute users_remove',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: pms_users_og access
  $permissions['pms_users_og access'] = array(
    'name' => 'pms_users_og access',
    'roles' => array(
      '0' => 'admin',
      '1' => 'director',
      '2' => 'leader',
      '3' => 'partner',
    ),
  );

  // Exported permission: user is partner
  $permissions['user is partner'] = array(
    'name' => 'user is partner',
    'roles' => array(
      '0' => 'director',
      '1' => 'leader',
      '2' => 'partner',
    ),
  );

  // Exported permission: view drupal profile
  $permissions['view drupal profile'] = array(
    'name' => 'view drupal profile',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: view ori users list
  $permissions['view ori users list'] = array(
    'name' => 'view ori users list',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: view ori users pages
  $permissions['view ori users pages'] = array(
    'name' => 'view ori users pages',
    'roles' => array(
      '0' => 'admin',
      '1' => 'director',
      '2' => 'leader',
      '3' => 'operator',
      '4' => 'partner',
    ),
  );

  // Exported permission: view users_disabled
  $permissions['view users_disabled'] = array(
    'name' => 'view users_disabled',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  return $permissions;
}
