<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function ori_regform_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: administer ori_regform
  $permissions['administer ori_regform'] = array(
    'name' => 'administer ori_regform',
    'roles' => array(),
  );

  // Exported permission: administer ori_regform tooltips
  $permissions['administer ori_regform tooltips'] = array(
    'name' => 'administer ori_regform tooltips',
    'roles' => array(),
  );

  return $permissions;
}
