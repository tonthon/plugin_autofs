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
<h2>Remove Local Mount Point</h2>
<table><tr><td>
<img src='images/drive.png' alt='drive'/>
</td><td style='text-align:left;vertical-align:top;'>
Are you sure you want to remove the following mount point : 
<br />
<br/>
{$mpointname} : {$mpoint}
<br/>
<br/>
<br/>
<b>You will also remove its associated map and the whole list of directories associated to it.</b>
<br/>
</td></tr><tr><td></td><td style='text-align:right'>
<input type='hidden' value='{$mpointname}' name='autofs_mountname' />
<input type='hidden' value='{$mpoint}' name='autofs_mountpoint' />
<input type='submit' name='_autofs_del_mpoint_posted' value='Remove' />
<input type='submit' name='autofs_cancel' value='Cancel' />
</td></tr></table>
</div>
