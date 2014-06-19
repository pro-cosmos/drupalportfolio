<?php
 
  
function ori_regform_settings(&$form_state){

  $prefix = variable_get('constant_ori_regform_prefix','');
 
  if(user_access('ori_regform add settings'))
  {
      $form['new_variable'] = array(
      '#type' => 'textfield',
      '#title' => t('Add new variable'),
      '#default_value' => '',
      '#description'=>t('Enter machine name of variable'),
      '#prefix'=>'<div style="clear:both">'    
       );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add' ),
        '#suffix'=>'</div>'  
      );
  }  
  
 $form['table_start'] = array(
    '#type' => 'markup',
    '#value' => "<table class='sticky-enabled tableSelect-processed sticky-table'>"
  );
 
 
  $settings = variable_get($prefix.'_settings_list','');
  if(!empty($settings))
  {
  $i=0;
  foreach($settings as $name=>$attr)
  {
   $i++;
    
   $form['fieldset'.$i] = array(
      '#title'=>  t(str_replace($prefix.'_', '', $name)), 
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => 1,
      '#prefix'=>'<tr><td>',
      '#suffix'=>'</td>', 
     );
         
      $form['fieldset'.$i][$name] = array(
      '#type' => 'textarea',
      '#title' => t($name),
      '#default_value' => variable_get($name,''),
       );
      
   if(user_access('ori_regform add settings'))
   {
      $form['delete|'.$name] = array(
          '#type' => 'checkbox',
          '#default_value' => '',
          '#title' => t('Delete'),
          '#prefix'=>'<td style="width:100px">',
          '#suffix'=>'</td></tr>', 
        );
   }
 }
 }
 
  $form['table_end'] = array(
    '#type' => 'markup',
    '#value' => "</table>"
  );
  
   
 $form['#submit'][] = 'ori_regform_settings_submit';
 
 return system_settings_form($form);
}

function ori_regform_settings_submit($form, &$form_state){

    $prefix = variable_get('constant_ori_regform_prefix','');
    $settings = array();  

  //save settings variable names to array
    foreach($form['#post'] as $name=>$val){
       
      //delete variables
      if(strpos('_'.$name,'delete|')>0){
         $del = explode('|',$name);
         variable_del($del[1]);
         unset($settings[$del[1]]);
      }
      elseif(strpos('_'.$name,$prefix)>0)
         { //save variables  
         $settings[$name] = $name; 
         }
      
      
    }
    
  
  //add new variable
  if(!empty($form['#post']['new_variable']))
     $settings[$prefix.'_'.(string)$form['#post']['new_variable']] = (string)$form['#post']['new_variable']; 

  variable_set($prefix.'_settings_list', $settings);
  
  
  
    //reset
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  // Exclude unnecessary elements.
  if ($op == t('Reset to defaults')) {
      $revert = array('ori_regform_feature' => array('variable'));
      module_load_include('inc', 'features', 'features.export');
      features_include();
      features_revert($revert); 
      
      drupal_goto('admin/settings/ori/ori_regform/settings');
  }
}



function ori_regform_tooltiplinks(&$form_state){
 global $user;   
 $prefix = variable_get('constant_ori_regform_prefix','');
 
 if($user->uid==1){
 
   $form[$prefix.'_tooltip_count'] = array(
      '#type' => 'select',
      '#title' => t('Кол-во ссылок'),
      '#default_value' => variable_get($prefix.'_tooltip_count', 3),
      '#options' =>  drupal_map_assoc(range(1,50)),  
       );
 }
 
 
 $group_options = variable_get('constant_ori_regform_tooltip_groups', ''); 
 for($i=1; $i<=variable_get($prefix.'_tooltip_count', 3);$i++)
 {
   $group_title =  '№'.$i. ' ';
   $group_title.=  (variable_get($prefix.'_tooltip_group'.$i, '') != '' && $user->uid==1)? '('.$group_options[variable_get($prefix.'_tooltip_group'.$i, '')].') ' : ""; 
   $group_title.= (variable_get($prefix.'_tooltip_link'.$i, '')? variable_get($prefix.'_tooltip_link'.$i, '') : 'текст ссылки №'.$i);

   $groupid =  variable_get($prefix.'_tooltip_group'.$i, 0);
   //none admin users allowed only group='important block'
   if($user->uid!=1 && $groupid==0 && !variable_get($prefix.'_tooltip_link'.$i, ''))
     $groupid = 'important';
   
   if($user->uid==1 || $user->uid!=1 && $groupid == 'important')
   {
       $form['fieldset'.$i] = array(
          '#title'=> $group_title, 
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => 1
         );

       
       
      $form['fieldset'.$i][$prefix.'_tooltip_group'.$i] = array(
        '#type' => 'select',
        '#title' => 'Группа',
        '#options' => $group_options, 
        '#default_value' =>$groupid
      );
      if($user->uid!=1){
        $form['fieldset'.$i][$prefix.'_tooltip_group'.$i] = array(
        '#type' => 'hidden',
        '#default_value' =>$groupid 
        );
      }
      
      
      $form['fieldset'.$i][$prefix.'_tooltip_link'.$i] = array(
        '#type' => 'textfield',
        '#title' => 'Ссылка',
        '#default_value' => variable_get($prefix.'_tooltip_link'.$i, ''),
      );

      $form['fieldset'.$i][$prefix.'_tooltip_title'.$i] = array(
        '#type' => 'textfield',
        '#title' => 'Заголовок',
        '#default_value' => variable_get($prefix.'_tooltip_title'.$i, ''),
      );

      $form['fieldset'.$i][$prefix.'_tooltip_text'.$i] = array(
      '#type' => 'textarea',
      '#title' => 'Текст описания ссылки',
      '#default_value' => variable_get($prefix.'_tooltip_text'.$i, ''),
       );
      }
  }
  
  
   $form['#submit'][] = 'ori_regform_tooltips_submit';
   return system_settings_form($form);
}



function ori_regform_tooltips_submit($form, &$form_state){

  //reset
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  // Exclude unnecessary elements.
  if ($op == t('Reset to defaults')) {
      $revert = array('ori_regform_tooltips_feature' => array('variable'));
      module_load_include('inc', 'features', 'features.export');
      features_include();
      features_revert($revert); 
      
      drupal_goto('admin/settings/ori/ori_regform/tooltip_links');
  }
  
}


