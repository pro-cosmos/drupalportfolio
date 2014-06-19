<?php


/**
 * Implementation of hook_group_operations_description().
 */
function subscription_group_operations_description(){

   if(user_access('execute mailsid_send'))
   {
      module_load_include('inc', 'pms_users', 'inc/subscription_mail/subscription.mail.inc');

      $out =  _subscription_mail_list_selected();
      $out.= '<p>'.l('Change selected mails', variable_get('subscription_baseurl','').'/mail').'</p>';
      $out = '<div id="mailsid_send" class="description">'.$out.'</div>';
   }


  return $out;
  }

/**
 *
 * @param type $op
 * @param type $args
 */
  function subscription_group_operations($op, $args = array()){

  switch($op){
     case 'users_delete':
         User::deleteUserAll(array_keys($args));
         break;
    }

  }



  /**
 * Implementation of hook_group_operations_callback().
 */
function subscription_group_operations_callback($op, $args = array()){

  switch($op){

         case 'instructions_send':
           if(user_access('execute instructions_send'))
           {
              $force = true;
              if (count($args['selected']) > variable_get('subscription_cron_send_limit', 300)) {
                drupal_set_message('Выбрано слишком большое кол-во пользователей, операция выполнится в фоновом режиме');
                $force = false;
              }
             if(is_array($args['selected'])){
               foreach($args['selected'] as $k=>$uid){
                  //submit mail with login and password by 6 event
                  subscription_user_send_event_mails($uid, variable_get('subscription_instructionsent_event',6), $force);
             }
             $message = t('User instructions mails send ('.count($args['selected']).')');
             }
          }
      break;



     case 'mailsid_send':
         if(user_access('execute mailsid_send'))
         {
           if(is_array($args['selected']) || in_array($args['selected'],array('filtered','all'))){
            module_load_include('inc', 'pms_users', 'inc/subscription_mail/subscription.mail.inc');

           $selectedMails = subscription_get_mails();
           $selectedUsers = $args['mode'];

           $message = subscription_mail_send_users($selectedUsers, $selectedMails);
           $message = array('ok'=>$message);

           //empty selected mail list
           unset($_SESSION['subscription_mail_list']);
           }
         }
         break;

     case 'mail_status_enable':
         if(user_access('execute mail_status_enable'))
         {
           if(is_array($args['selected'])){
             foreach($args['selected'] as $k=>$uid){
                 subscription_user_set_status($uid, 1);
           }

           $message = t('User mails has enabled ('.count($args['selected']).')');
           }
         }
         break;

     case 'mail_status_disable':
         if(user_access('execute mail_status_disable'))
         {
           if(is_array($args['selected'])){
             foreach($args['selected'] as $k=>$uid){
                 subscription_user_set_status($uid, 0);
             }

           $message = t('User mails has disabled ('.count($args['selected']).')');
           }
         }
         break;
    }


return $message;
}



  /**
 * Implementation of hook_group_operations_info().
 */
function subscription_group_operations_info($arg){
   $info = array();

    if(user_access('execute instructions_send') ){
      $info['instructions_send'] = array(
              'title'=>t('Send instructions mails'),
              'button'=>array('#title' =>t('Send instructions'),'#post_render'=>array('_gaction_button_instructions_send_post_render'))
              );
    }


    if(user_access('execute mailsid_send') ){
      $info['mailsid_send'] = array(
              'title'=>t('Send selected mails to recipients')
              );
    }

    if(user_access('execute mail_status_enable'))
      $info['mail_status_enable'] = array(
              'title' =>t('Activate mails'),
              'button'=>array('#title' =>t('Activate mails'),'#post_render'=>array('_gaction_button_mail_status_post_render'))
              );

   if(user_access('execute mail_status_disable'))
      $info['mail_status_disable'] = array(
              'title' =>t('Disable mails'),
              'button'=>array('#title' =>t('Disable mails'),'#post_render'=>array('_gaction_button_mail_status_post_render'))
              );


  return $info;
  }



/**
 *
 * @param type $content
 * @param type $elements
 * @return string
 */
  function _gaction_button_mail_status_post_render($content, $elements){

    if($elements['#title'] == t('Disable mails') && !$elements['#context']['mail_status'])
        $content = '';
    if($elements['#title'] == t('Activate mails') && $elements['#context']['mail_status'])
        $content = '';

    return $content;
}


/**
 *
 * @param type $content
 * @param type $elements
 * @return string
 */
  function _gaction_button_instructions_send_post_render($content, $elements){

    if((int)$elements['#context']['instruction_send']>1){
       $content = str_replace(' button_instructions_send',' button_instructions_send1', $content);
    }
    return $content;
}

