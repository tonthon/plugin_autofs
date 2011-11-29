<style>
{literal} 
.mountrep{
    padding:5px;
    margin:5px;
    border:1px solid #555;
    display:inline-block;
}
.mpoint{
    border-bottom:1px solid #666;
    padding:5px;
}
{/literal} 
</style>
<h2>{t}NFS Drive Configuration{/t}</h2>
<p>
{t}Configured Mountpoints{/t}
</p>
{section name=mountlist loop=$mountPoints}
{assign var=mountpoint value=$mountPoints[mountlist]}
<div class='mpoint'>
<h2>{$mountpoint->name} ( {$mountpoint->point} )</h2>
<input class="center" type="image" style="padding:1px" name="autofs_mpoint_rm_{$mountpoint->name}" title="Remove mountpoint" src="images/lists/trash.png">
<br />
{section name=replist loop=$mountpoint->mreps}
{assign var=mountrep value=$mountpoint->mreps[replist]}
<span class='mountrep'>
<b>Chemin à monter : </b>{$mountrep->path}  <br /><br />
<b>Server : </b>{$mountrep->server}<br />
<b>Remote rep : </b>{$mountrep->remote_path}<br />
<b>Mount options : </b>{$mountrep->options}<br />
</span>
{/section}
</div>
<input type='submit' name='autofs_edit_form_mpoint_{$mountpoint->name}' value='Edit' />
<input type='submit' name='autofs_del_mpoint_{$mountpoint->name}' value='Remove' />
{/section}
<input type='submit' name='autofs_add_form_mpoint' value='Add a new mountpoint' />
<div>
{$message}
</div>
