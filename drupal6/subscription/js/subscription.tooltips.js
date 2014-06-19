var fields = Array();
$(document).ready(function (){
  if (fields == undefined){
    fields = new Array();
  }
  for (var field in fields){
    field = fields[field];
    tip = $('<span></span>').attr('class', 'tip-field').attr('title', field.text);
    tip.tooltip({
      showURL: false
    });
    field.element()[field.mode](tip);
  }
});
