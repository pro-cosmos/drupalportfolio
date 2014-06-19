$(document).ready(function () {
  $('#add-selected').click(function (){
    data = "";
    $('input.select-user:checked').each(function(index, element){
      data += $(element).attr('id') + "-";
    });
    if (data == "") return false;
    //alert(data);
    $.get('/admin/oriflame/subscription/mail/addreceiver', {'ids': data}, replace_users_block);
    return false;
  });
  $('input.select-all-users').change(function (){
   checked = $(this).attr('checked');
    $('input.select-user').each(function(index, element){
      if (checked == false){
	      $(element).removeAttr('checked');
      }
      else { 
        $(element).attr('checked', 'checked');
      }
    });
  });
  delay = 8000;
  $('#send-all-button').click(function (){
    $.get($(this).attr('href'), function (data) {go_to_mail_list();});
    setTimeout('go_to_mail_list()', delay);
    return false;
  });
  $('#send-selected-button').click(function (){
    $.get($(this).attr('href'), function (data) {go_to_mail_list();});
    setTimeout('go_to_mail_list()', delay);
    return false;
  });
  $('#clear-selected').click(function (){
    $.get($(this).attr('href'), function (data) {
      $('#selected-users-div').html('');
      update_send_selected_link();
    });
    return false;
  });
  update_send_selected_link();
});

function go_to_mail_list(){
  window.location.replace("/admin/subscription/mail");
}

function replace_users_block(data){
  //alert('received: ' + data);
  elem = Drupal.parseJson(data)['data'];
  if (elem == '') return;
  $('#selected-users-div').append($(elem));
  update_send_selected_link();
}
function update_send_selected_link(){
  if ($('#selected-users-div span').length == 0){
    $('#send-selected-button').hide();
    $('#send-all-button').show();
    $('#clear-selected').hide();
  }
  else {
    $('#send-selected-button').show();
    $('#send-all-button').hide();
    $('#clear-selected').show();
  }
}
function deleteUser(element){
  $.get('/admin/oriflame/subscription/mail/removereceiver', {'id': $(element).attr('id')}, 
    function(data) {
      $(element).parent().remove();
     // alert(data);
      update_send_selected_link();
    });
  return false;
}
