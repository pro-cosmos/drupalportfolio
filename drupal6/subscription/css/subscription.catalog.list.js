$(document).ready(function () {
  $('.weight-holder').parent().addClass('dragHandle');
  $('#change-countries-weight-button').click(function () {
    order = $('table.sticky-enabled').tableDnDSerialize();
    $.get($(this).attr('href'), {'order' : order}, function (data) {
      status = Drupal.parseJson(data)['status'];
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
