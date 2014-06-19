<?php

/**
 * Implementation of hook_views_data()
 */
function pms_users_views_data() {
  // ori_user table
  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['ori_user']['table']['group'] = t('PMS Users');
  $data['ori_user']['table']['base'] = array(
    'field' => 'id',
    'title' => t('PMS Users'),
    'help' => t('Contains fields that belong to ori_user table'),
  );

  $data['ori_user']['id'] = array(
    'title' => t('User ID'),
    'help' => t("The Identifier of a partner"),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ori_user']['uid'] = array(
    'title' => t('User UID'),
    'help' => t("Drupal user UID that is related to partner"),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ori_user']['psid'] = array(
    'title' => t('Sponsor UID'),
    'help' => t("Sponsor's UID of a partner"),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ori_user']['email'] = array(
    'title' => t('Partner Email'),
    'help' => t("The email of a partner"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_string',
    ),
  );

  $data['ori_user']['firstname'] = array(
    'title' => t('Partner First Name'),
    'help' => t("The first name of a partner"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_string',
    ),
  );

  $data['ori_user']['middlename'] = array(
    'title' => t('Partner Middle Name'),
    'help' => t("The middle name of a partner"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_string',
    ),
  );

  $data['ori_user']['lastname'] = array(
    'title' => t('Partner Last Name'),
    'help' => t("The last name of a partner"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_string',
    ),
  );

  $data['ori_user']['birthday'] = array(
    'title' => t('Partner Birthday'),
    'help' => t('Partner\'s birthday.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  $data['ori_user']['regdate'] = array(
    'title' => t('Registration Date'),
    'help' => t('Partner\'s registration date.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
      'label' => t('Registered on'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  
  $data['ori_user']['status'] = array(
    'title' => t('Partner Status'),
    'help' => t('Partner\'s status.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}