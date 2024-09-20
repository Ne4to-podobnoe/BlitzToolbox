; Blib - A part of BlitzToolbox
; Zip Utils for Blitz3D.
; v1.01 2023.11.5
; https://github.com/ZiYueCommentary/BlitzToolbox

.lib "Blib.dll"

BlibCreateZip_%(file$, password$):"_BlibCreateZip@8"
BlibOpenZip_%(file$, password$):"_BlibOpenZip@8"
BlibZipAdd(hzip%, dst$, src$):"_BlibZipAdd@12"
BlibZipRemove(hzip%, dst$):"_BlibZipRemove@8"
BlibZipAddFolder(hzip%, dst$):"_BlibZipAddFolder@8"
BlibCloseZip(hzip%):"_BlibCloseZip@4"
BlibGetZipItem%(hzip%, index%):"_BlibGetZipItem@8"
BlibUnzipItem(hzip%, index%, zipentry%, dst$):"_BlibUnzipItem@16"
BlibFindZipItem%(hzip%, name$, ic%, zipentry%):"_BlibFindZipItem@16"
BlibSetUnzipBaseDir(hzip%, dir$):"_BlibSetUnzipBaseDir@8"
BlibZipEntryName$(zipentry%):"_BlibZipEntryName@4"
BlibUnzip(zip$, dst$, password$):"_BlibUnzip@12"