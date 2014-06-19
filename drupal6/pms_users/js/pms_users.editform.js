Drupal.behaviors.pms_users_editform = function(context) {

  settings = Drupal.settings.pms_users_editform ;

  var passvalue = settings.pass;
  var PASS = $('#edit-user-password').val(settings.pass);
  var textShow = 'Изменить';
  var textHide = 'Скрыть';
  var SWUTCHLINK = '<div style="float:left;padding:4px"><span class="ajaxlink" id="switchpass">'+textShow+'</span></div>';
  var PASSTEXT = $('<input name=\'user_password\' type=\'text\' id=\'edit-user-password-text\' class=\'form-text fluid\' style=\'float:left\' />');


  /* init */

  $('#edit-psid-edit-button').click(function(){
    $('#edit-psid-edit-wrapper').toggle();
    $('#psid_edit_descr').toggle();
    return false;
  });



  $(SWUTCHLINK).insertAfter('#edit-user-password-wrapper');
  var PASSSWITCH = $('#switchpass');
  PASSTEXT.insertAfter('#edit-user-password').val(PASS.val()).hide();


  PASSSWITCH.live('click', function(){

    var exist = ($('#edit-user-password-text').length>0)? true:false;
    var show = (exist && $('#edit-user-password-text').css('display')=='block')? true:false;


    if(show){
      //$('#edit-user-password-text').remove();
      $('#edit-user-password-text').hide();
      $(this).html(textShow);
      PASS.show();
    }
    else{
      //if(!exist)
      //   PASSTEXT.insertAfter('#edit-user-password').val(PASS.val());

      $('#edit-user-password-text').show();
      $(this).html(textHide);
      PASS.hide();
    }

  });

  //add event handler to dinamically created element
  $('#edit-user-password-text').live('change',function(){
    PASS.val($(this).val());
  });



}

