<?php


/**
 * Define all relativity views handlers
 *
 **/
function subscription_views_handlers() {
  return array(
    'info' => array(
         'path' => drupal_get_path('module', 'subscription').'/views/handlers'
    ),
    'handlers' => array(
      // filter handlers
      'ori_views_handler_filter_value_in_operator' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),

      'ori_views_handler_filter_substitle_in_operator' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),

    )//handler
  );
}



function subscription_views_data() {

        // user group info fields
  $tables['catalog'] = array(
    'table' => array(
          'group' => t('Subscription:Catalog')
        ),
    'id' => array(
          'title' => t('Catalog id'),
          'help' => t('Catalog ID'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_numeric',
          ),
    ),
   'type' => array(
          'title' => t('Catalog Type'),
          'help' => t('Catalog Type'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_string',
          ),
    ),
       'value' => array(
          'title' => t('Catalog Value'),
          'help' => t('Catalog Value'),
         'field' => array(
            'handler' => 'views_handler_field',
          ),
         'filter' => array(
            'handler' => 'ori_views_handler_filter_value_in_operator',
          ),
         'sort' => array(
           'handler' => 'views_handler_sort',
          ),
    )

    );


      // user group info fields
  $tables['subscription'] = array(
    'table' => array(
          'group' => t('Subscription')
        ),
    'id' => array(
          'title' => t('id'),
          'help' => t('Subscription ID'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_numeric',
          ),
    ),
   'title' => array(
          'title' => t('Subscription title'),
          'help' => t('Subscription title'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'ori_views_handler_filter_substitle_in_operator',
          )
    )

    );



      // user group info fields
  $tables['substatus'] = array(
      'table' => array(
          'group' => t('Substatus'),
          'join' => array(
            'subscription' => array (
              'field' => 'sid',
              'table' => 'substatus',
              'left_table' => 'subscription',
              'left_field' => 'id'
              ),
             'catalog' => array (
              'field' => 'stid',
              'table' => 'substatus',
              'left_table' => 'catalog',
              'left_field' => 'id'
              ),
          ),
      ),
      'uid' => array(
          'title' => t('uid'),
          'help' => t('Subscriber uid'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_numeric',
          ),
    ),
    'sid' => array(
          'title' => t('sid'),
          'help' => t('Subscription ID'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_numeric',
          ),
        'relationship' => array(
            'label' => t('Subscription relationship'),
            'title' => t('Subscription relationship'),
            'help' => t('Subscription relationship'),
            'base' => 'subscription',
            'base field' => 'id',
            'field' => 'sid',
            'left_table' => 'subscription',
            'left_field' => 'id'
        )
    ),
    'stid' => array(
          'title' => t('stid'),
          'help' => t('Subscription status'),
          'field' => array(
            'handler' => 'views_handler_field',
          ),
          'filter' => array(
            'handler' => 'views_handler_filter_numeric',
          ),
        'relationship' => array(
            'label' => t('Substatus-Catalog relationship'),
            'title' => t('Substatus-Catalog relationship'),
            'help' => t('Substatus-Catalog relationship'),
            'base' => 'catalog',
            'base field' => 'id',
            'field' => 'stid',
            'left_table' => 'catalog',
            'left_field' => 'id'
            )
    )

    );


  $tables['ori_user'] =  array(
    'table' => array(
      'join' => array(
        'substatus' => array (
          'field' => 'id',
          'table' => 'ori_user',
          'left_table' => 'substatus',
          'left_field' => 'uid'
          ),
        ),
      ),
    'id' => array(
      'relationship' => array(
        'label' => t('Substatus relationship'),
        'title' => t('Substatus relationship'),
        'help' => t('Substatus relationship'),
        'base' => 'substatus',
        'base field' => 'uid',
        'field' => 'id',
        'left_table' => 'substatus',
        'left_field' => 'uid'
        ),
      ),
  );

   /*
  $tables['substatus'] =  array(
    'table' => array(
      'group' => t('Ori'),
      'join' => array(
        'ori_user' => array (
          'field' => 'uid',
          'table' => 'substatus',
          'left_field' => 'id',
          'left_table' => 'ori_user',
          ),
        ),
      ),
    'uid' => array(
      'relationship' => array(
        'label' => t('ori_user relationship'),
        'title' => t('ori_user relationship'),
        'help' => t('ori_user relationship'),
        'base' => 'substatus',
        'field' => 'uid',
        'left_table' => 'ori_user',
        'left_field' => 'id'
        ),
      ),
  );
*/


return $tables;
}