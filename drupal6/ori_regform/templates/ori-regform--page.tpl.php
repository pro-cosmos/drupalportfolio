<?
extract($vars);
?> 

<div id="steps" class="clear-block">
<ul>
    <li class="step1"><?=$step1?></li>
    <li class="step2"><?=$step2?></li>
    <li class="step3"><?=$step3?></li>
</ul>
</div>

<div id="important_block">
<h2>ВАЖНО</h2>
<span><b>Мы гарантируем безопасность ваших данных. Все данные хранятся на специально защищенном веб-сервере.</b></span>
<?=$important_content ?>
</div>

<div id="ori_regform_page_content">
<h2>Анкета</h2>
<?=$content?>
</div>


<div id="agreements"><?=$agreements?></div>