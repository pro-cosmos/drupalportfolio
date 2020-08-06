(function ($, Drupal) {
  Drupal.behaviors.group_visible = {
    attach: function attach(context, settings) {

      var websiteInp = $('input[name="field_pog_info_channel[Webseite]"]'),
          websiteTarget = $('.websiteTarget');

      var turn = function(source, target) {
        if (target.length > 0) {
          if (source) target.show();
          else target.hide();
        }
      };

      if (websiteInp.length > 0) {
        turn(websiteInp.is(':checked'), websiteTarget)
        websiteInp.once().on('change', function() {
          turn(websiteInp.is(':checked'), websiteTarget)
        });
      }

      // Hide wrapper if no visible data inside
      var $wrapper = $('form .pog-fields-wrapper');

      var checkVisible = function(data) {
        var check = false;

        data.each(function() {
          if ($(this).is(':visible')) {
            check = true;
            return false;
          }
        });

        return check;
      };

      $wrapper.each(function() {
        var $this = $(this),
            $data = $this.find('input, textarea, label');

        if (checkVisible($data)) {
          $this.show();
        } else {
          $this.hide();
        }
      });

    }
  };

  Drupal.behaviors.second_level_checkbox = {
    attach: function attach(context, settings) {

      var $source = $('input[name="field_pog_website_needs[3]"]'),
        $target = $('input[name="field_pog_website_needs[31]"]');

      if ($source.length) {
        $source.once().on('change', function() {
          if ($(this).is(':checked')) {
            $target.parent('.form-item').show();
          } else {
            $target.prop('checked', false).parent('.form-item').hide();
          }
        });
      }

    }
  };

})(jQuery, Drupal);
