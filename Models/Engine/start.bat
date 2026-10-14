@echo off
chcp 65001 > nul

start "" "http://localhost:8000/index.htm"
set "current_dir=%~dp0"
set "current_dir=%current_dir:\=\\%"

powershell -WindowStyle Hidden -Command "$p=8000; $v='%current_dir%'; $l=New-Object System.Net.HttpListener; $l.Prefixes.Add(\"http://localhost:$p/\"); $l.Start(); while($l.IsListening) { try { $c=$l.GetContext(); $q=$c.Request.Url.LocalPath.Trim('/'); if(!$q){$q='index.htm'}; $f=[System.IO.Path]::Combine($v,$q); if(Test-Path $f -PathType Leaf){ $b=[System.IO.File]::ReadAllBytes($f); $ext=[System.IO.Path]::GetExtension($f).ToLower(); $ct='text/plain'; if($ext -eq '.htm' -or $ext -eq '.html'){$ct='text/html'} elseif($ext -eq '.js' -or $ext -eq '.mjs'){$ct='application/javascript'} elseif($ext -eq '.css'){$ct='text/css'} elseif($ext -eq '.png'){$ct='image/png'} elseif($ext -eq '.json'){$ct='application/json'}; $c.Response.ContentType=$ct; $c.Response.OutputStream.Write($b,0,$b.Length) } else { $c.Response.StatusCode=404 }; $c.Response.Close() } catch {} }"
