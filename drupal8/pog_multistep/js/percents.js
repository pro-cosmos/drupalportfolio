(function ($, Drupal) {
  Drupal.behaviors.percents = {
    attach: function attach(context, settings) {
      var $percents = $('.calc-percents'),
          $alertModal = $('<div id="percentAlert" class="modal fade"><div class="modal-dialog"><div class="modal-content"><div class="modal-header">'+
                  '<button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">×</span></button><h4 class="modal-title">'
                  + Drupal.t('The amount must be less than 100') + '</h4></div>'+
                  '</div></div></div>');

      $percents.each(function() {
        if ($percents.hasClass('percents-processed')) {
          return;
        }

        $percents.addClass('percents-processed');

        var $this = $(this);

        var $input = $this.find('input.form-number');

        $input.on('focusout', function() {
          setTimeout(function() {
            if (!$input.is(":focus")) {
              calc_percent($this);
            }
          });
        });
      });

      var calc_percent = function($this) {
        var $input = $this.find('input.form-number'),
          data = [],
          digits = 0,
          summ = 0;

        $input.each(function(i, el) {
          data[i] = parseInt($(el).val());
          if (!isNaN(data[i])) {
            digits++;
            summ += data[i];
          }
        });


        if (summ > 100) {
          // Сумма должна быть меньше 100 !!
            $($alertModal).modal('show');
          $input[0].focus();
          $input.closest('.form-item').addClass('has-error');
          return;
        } else {
            $input.closest('.form-item').removeClass('has-error');
        }

        switch (digits) {
          case 3:
            if (summ < 100) {
              var k = 100 / summ;
              data[0] = parseInt(data[0] * k);
              data[1] = parseInt(data[1] * k);
              data[2] = 100 - data[0] - data[1];
              $input.each(function(i, el) {
                $(this).val(data[i]);
              });
            }

            break;

          case 2:
            var summ = 100, k;
            data.forEach(function(el, i) {
              if (!isNaN(el)) {
                summ -= el;
              } else {
                k = i;
              }
            });
            data[k] = summ;
            $input.each(function(i, el) {
              $(this).val(data[i]);
            });
            break;

          case 1:
            var summ = 100, v = [], k = 0;
            data.forEach(function(el, i) {
              if (!isNaN(el)) {
                summ -= el;
                k = i;
              }
            });
            v[0] = parseInt(summ / 2);
            v[1] = summ - v[0];
            k=0;
            data.forEach(function(el, i) {
              if (isNaN(el)) {
                data[i] = v[k];
                k++;
              }
            });
            $input.each(function(i, el) {
              $(this).val(data[i]);
            });

            break;
        }
      };
    }
  };
})(jQuery, Drupal);