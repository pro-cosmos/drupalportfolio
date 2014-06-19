$(document).ready(function () {

   $('#edit-pagesize').change(function(){
      $('#subscription-mail-list-form').submit()
   } );



  $('#send-selected-mail-button').click(function (){
    data = "";
    $('input.select-mail:checked').each(function(index, element){
      if (data != ""){
	      data += "-";
      }
      data += $(element).attr('id');
    });
    if (data == "") return false;
    href = $(this).attr('href');
    href = href.replace('mailsid', data);
    window.location.href = href;
    return false;
  });


  $('.weight-holder').parent().addClass('dragHandle');



  $('#edit-saveorder').click(function () {
    var order = $('table.sticky-enabled').tableDnDSerialize();
    var url = Drupal.settings.basePath + '/admin/oriflame/subscription/mail/changeorder';
    url = url.replace('//','/');

    $.get(url, {'page':window.location.search, 'order' : order}, function (data) {
      status = Drupal.parseJson(data)['status'];
      alert('Порядок сохранен');
    });
    return false;
  });

  $('table.sticky-enabled').tableDnD({
    dragHandle: "dragHandle"
  });
  $("table.sticky-enabled tbody tr").hover(function() {
    $(this.cells[0]).css('cursor', 'move');
  }, function() {
    $(this.cells[0]).css('cursor', 'default');
  });
});
