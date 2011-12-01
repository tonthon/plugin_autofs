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
.mpoint{
padding:5px;
margin:20px;
display:block;
color:#00378A;
background-color:#e5Ebf0;
}
.mountrep{
    padding:5px;
    margin:5px;
    display:inline-block;
    color:#00378A;
}
.mpoint img{
    margin-right:5px;
    display:block;
}
{/literal}
</style>
<div class='autofs_main'>
<h2>{t}NFS Drive Configuration{/t}</h2>
<p>
{t}Configured Mountpoints{/t}
</p>
{section name=mountlist loop=$mountPoints}
{assign var=mountpoint value=$mountPoints[mountlist]}
<span class='mpoint round'>
<table cellpadding='0' cellspacing='0'><tr><td>
<h2>{$mountpoint->name} ( {$mountpoint->point} )</h2>
<img src='images/drive.png' />
<input type='submit' name='autofs_edit_form_mpoint_{$mountpoint->name}' value='Edit' />
<input type='submit' name='autofs_del_mpoint_{$mountpoint->name}' value='Remove' />
</td><td>
{section name=replist loop=$mountpoint->mreps}
{assign var=mountrep value=$mountpoint->mreps[replist]}
<span class='mountrep drop-shadow round'>
<table><tr><td>
<img src='images/folder.png' />
</td><td>
<b>Local path : </b>{$mountrep->path}  <br /><br />
<b>Server : </b>{$mountrep->server}<br />
<b>Remote rep : </b>{$mountrep->remote_path}<br />
<b>Mount options : </b>{$mountrep->options}<br />
<input type='submit' name='autofs_edit_mrep_{$mountpoint->name}___{$mountrep->id}' value='Edit'/>
<input type='submit' name='autofs_del_mrep_{$mountpoint->name}___{$mountrep->id}' value='Remove'/>
</td></tr></table>
</span>
{/section}
<input type='submit' name='autofs_add_mrep_{$mountpoint->name}' value='Add a directory' />
</td></tr></table>
</span>
{/section}
<br />
<br />
<input type='submit' name='autofs_add_form_mpoint' value='Add a new mountpoint' />
</div>
<div>
{foreach from=$message item=mess}
{$mess}
{/foreach}
</div>
