{debug}
<h2>{t}NFS Drive Configuration{/t}</h2>
<p>
{t}Liste des montages configurés{/t}
</p>
<table>
<thead>
<tr>
<th>Adresse Ip du serveur NFS</th>
<th>Répertoire à monter</th>
<th>Point de montage</th>
<th>Option de montage</th>
</tr>
</thead>
<tbody>
{section name=mountlist loop=$mountPoints}
{assign var=mountpoint value=$mountPoints[mountlist]}
<tr>
<td>{$mountpoint->server}</td>
<td>{$mountpoint->rep}</td>
<td>{$mountpoint->point}</td>
<td>{$mountpoint->options}</td>
</tr>
{/section}
</tbody>

</table>

<p class="plugbottom">
&nbsp;
</p>
