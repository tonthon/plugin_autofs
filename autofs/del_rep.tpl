<style>
{literal}
h2, p, .autofs_main{
color:#00378A;
}
.round{
    -moz-border-radius:4px;
    border-radius:4px;
}
.drop-shadow {
    -webkit-box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 60px rgba(0, 0, 0, 0.1) inset;
    -moz-box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 40px rgba(0, 0, 0, 0.1) inset;
    box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 40px rgba(0, 0, 0, 0.1) inset;

}
.autofs_main img{
    margin-right:5px;
    display:block;
}
{/literal}
</style>
<div class='autofs_main'>
<h2>Remove Directory</h2>
<table><tr><td style='vertical-align:top'>
<img src='images/folder.png' />
</td><td style='text-align:right;vertical-align:top'>
Are you sure you want to remove the following Directory : <br />
{$path}<br/>
From the following map : <br />
auto.{$mapname}
<br />
<input type='hidden' value='{$path}' name='autofs_rep_path' />
<input type='hidden' value='{$mapname}' name='autofs_mapname' />
<input type='submit' name='_autofs_del_form_rep_posted' value='Remove' />
<input type='submit' name='autofs_cancel' value='Cancel' />
</td></tr></table>
</div>
<div>
{foreach from=$message item=mess}
{$mess}
{/foreach}
</div>
