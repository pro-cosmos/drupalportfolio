<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function subscription_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: administer mail and subscription
  $permissions['administer mail and subscription'] = array(
    'name' => 'administer mail and subscription',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: administer subscription mails
  $permissions['administer subscription mails'] = array(
    'name' => 'administer subscription mails',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: administer subscription options
  $permissions['administer subscription options'] = array(
    'name' => 'administer subscription options',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: administer subscription requests
  $permissions['administer subscription requests'] = array(
    'name' => 'administer subscription requests',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: administer subscription tooltips
  $permissions['administer subscription tooltips'] = array(
    'name' => 'administer subscription tooltips',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: administer subscription variables
  $permissions['administer subscription variables'] = array(
    'name' => 'administer subscription variables',
    'roles' => array(
      '0' => 'admin',
    ),
  );

  // Exported permission: execute instructions_send
  $permissions['execute instructions_send'] = array(
    'name' => 'execute instructions_send',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute mail_status_disable
  $permissions['execute mail_status_disable'] = array(
    'name' => 'execute mail_status_disable',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute mail_status_enable
  $permissions['execute mail_status_enable'] = array(
    'name' => 'execute mail_status_enable',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: execute mailsid_send
  $permissions['execute mailsid_send'] = array(
    'name' => 'execute mailsid_send',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  // Exported permission: view ori subscription pages
  $permissions['view ori subscription pages'] = array(
    'name' => 'view ori subscription pages',
    'roles' => array(
      '0' => 'admin',
      '1' => 'operator',
    ),
  );

  return $permissions;
}
