(function ($) {

  Drupal.behaviors.ruswebs_support = {
    attach: function (context, settings) {

      // console.log(settings);
      var $submitbtn = $('#edit-submit--3');
      $submitbtn.bind('click', function(e) {

        //$('input[name=url]').val('test1');
          screenShot(document.body);
        $('#ruswebs-support-feedback-form').submit();

      });

      $('#ruswebs-support-feedback-form').bind('submit', function(){
      //  screenShot(document.body);
      //$('input[name=url]').val('test2');
      //  $(this).ajaxSubmit(options);
      //     return false;
      });

      
      //  Drupal.ajax['ruswebs-support-feedback-form'].options.beforeSubmit = function(form_values, element, options) {
      //    alert('submit 1');
      // your code
      //  }


      function screenShot(id) {
        html2canvas(id, {
          onrendered: function(canvas) {
            var img = canvas.toDataURL("image/png");
            var output = img.replace(/^data:image\/(png|jpg);base64,/, "");
            var output = encodeURIComponent(img);
            //alert(output);
            $('input[name=screenshot]').val(output);
          }
        });
      }

    }
  };


}(jQuery));