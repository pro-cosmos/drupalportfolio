$(document).ready(function (){
    $('.collapser').click(function(){
        $this = $(this);
        $f = $this.parent().parent();
        $t = $f.find('fieldset-wrapper');
        if (!$f.hasClass('collapsed')){
            $t.hide('fast', function (){
                $f.addClass('collapsed');
            });
        }
        else{
            $f.removeClass('collapsed');
            $t.show('fast');
        }
        return false;
    });
});
