<?php

/**
 * Implements hook_drush_command().
 */
function tbg_base_drush_command() {
  return array(
    'tbg-base-im' => array(
      'description' => 'Установка модулей',
      'drupal dependencies' => array('tbg_base'),
      'aliases' => array('tbim'),
      'callback' => 'drush_tbg_base_im_callback',
    ),
    'tbg-base-if' => array(
      'description' => 'Установка features',
      'drupal dependencies' => array('tbg_base'),
      'aliases' => array('tbif'),
      'callback' => 'drush_tbg_base_if_callback',
    ),
  );
}

/**
 * Import users from the old site.
 */
function drush_tbg_base_im_callback() {

  ini_set('memory_limit', '1024M');
  // Enable ALL modules
  _tbg_base_update_modules(variable_get('tbg_base_install_modules', array()));

  $message = 'tbg_base install modules ok';
  drush_print($message);
}


/**
 * Import users from the old site.
 */
function drush_tbg_base_if_callback() {

  ini_set('memory_limit', '1024M');
   // Enable ALL features modules
  _tbg_base_update_modules(variable_get('tbg_base_install_features', array()));
  // Revert ALL features
  _tbg_base_revert_features();
  // Flush all caches.
  //drupal_flush_all_caches();

  $message = 'tbg_base install features ok';
  drush_print($message);
}
