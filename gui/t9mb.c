#include "t9mb.h"

#if 1
//"Æ´ÒôÊäÈë·¨ºº×ÖÅÅÁĞ±í" 
const static unsigned char PY_mb_ex_space [] = {""}; 
const static unsigned char PY_mb_ex_a[]=		{"°¡°¢ß¹àÄëçï¹ºÇ°²°´°®°µ°£"}; 
const static unsigned char PY_mb_ex_ai[]=		{"°®°£°­°«°¤°¦°¥°§°¨°©°ª°¬°¯´ôŞßàÈàÉæÈè¨êÓíÁïÍö°"}; 
const static unsigned char PY_mb_ex_an[]=		{"°²°´°µ°¶°¸°³°±°·°°ÚÏÛûŞîáíâÖèñï§ğÆ÷ö¹ã³§"}; 
const static unsigned char PY_mb_ex_ang[]=		{"°º°¹°»"}; 
const static unsigned char PY_mb_ex_ao[]=		{"°¼°Â°½°¾°¿°À°Á°Ã°ÄÏùÛêŞÖà»á®âÚåÛæÁæñéáñúòüöË÷¡÷é"}; 
const static unsigned char PY_mb_ex_b[]=		{"²»²¿±¾±«°Ø±Ï±ä±È²¢±ğ°Ù±¨²½°ì±í"}; 
const static unsigned char PY_mb_ex_ba[]=		{"°Ñ°Ë°É°Í°Î°Ô°Õ°Ö°Ó°Å°Æ°Ç°È°Ê°Ì°Ï°Ğ°ÒÜØİÃá±å±îÙôÎöÑ÷É"}; 
const static unsigned char PY_mb_ex_bai[]=		{"°Ù°×°Ü°Ú²®°Ø°Û°İ°ŞŞãêş"}; 
const static unsigned char PY_mb_ex_ban[]=		{"°ì°ë°å°à°ã°æ°è°á°ß°â°é°ä°ç°ê°íÚæÛàîÓñ£ñ­ô²"}; 
const static unsigned char PY_mb_ex_bang[]=	{"°ï°ô°î°ñ°ğ°ò°ó°õ°ö°÷°ø°ùİòäº"}; 
const static unsigned char PY_mb_ex_bao[]=		{"±¨±£°ü°ş±¡°û±©±¦±¥±§±¬ÅÚ±¤ÅÙ°ú°ı±¢±ª±«ÆØÆÙİáæßìÒğ±ñÙõÀöµÙèå²"}; 
const static unsigned char PY_mb_ex_bei[]=		{"±±±»±¶±¸±³±²±´±­±°±Û±¯±®±µ±·±¹±ºØÃÚéÚıÛıİÉİíßÂã£íÕğÇñØöÍ÷¹"}; 
const static unsigned char PY_mb_ex_ben[]=		{"±¾±¼±½±¿º»ÛÎÛĞêÚï¼"}; 
const static unsigned char PY_mb_ex_beng[]=	{"±Ã±À°ö±Á±Â±Ä±ÅàÔê´"}; 
const static unsigned char PY_mb_ex_bi []=		{"±È±Ø±Ü±Õ±Ù±Ê±Ú±Û±Ï±Ë±Æ±Ò±Ç±ÎÃØ±É±Ì±Í±Ğ±Ñ±Ó±Ô±Ö±×±İ·÷ÃÚØ°ÙÂÛıÜÅÜêİ©İÉŞµßÁßÙáùâØã¹ääå¨åöåşæ¾æÔèµêÚî¢î¯îéïõñÔóÙóëó÷ô°ôÅõÏ÷Â"}; 
const static unsigned char PY_mb_ex_bian[]=	{"±ä±ß±ã±à±é±ç±â±æ±Ş±á±å±èØÒÛÍÜĞâíãêçÂì®ìÔí¾íÜñ¹ñÛòùóÖöı±Ë°¶"}; 
const static unsigned char PY_mb_ex_biao[]=	{"±í±ê±ë±ìÉ×æ»æôè¼ì©ì­ïÚïğñ¦ñÑ÷§÷Ô"}; 
const static unsigned char PY_mb_ex_bie[]=		{"±ğ±î±ï±ñõ¿"}; 
const static unsigned char PY_mb_ex_bin[]=		{"±ö±ò±ó±ô±õ±÷ÙÏáÙçÍçãéÄéëë÷ïÙ÷Æ÷Ş"}; 
const static unsigned char PY_mb_ex_bing[]=	{"²¢²¡±ø±ú±ù±û±ı±ü±şÆÁÙ÷ÚûŞğéÄ"}; 
const static unsigned char PY_mb_ex_bo[]=		{"²¨²¥°ş²®²¦±¡²©²ª²µ²£²´°Ø²·²¤²§²«²¬²­²¯²°²±²²²³ÆÇØÃÙñà£âÄéŞêşë¢íçîàğ¾ô¤õÀõËõÛ"}; 
const static unsigned char PY_mb_ex_bu[]=		{"²»²¿²½²¼²¹²¶ÆÒ²·±¤²¸²º²¾²Àß²åÍê³êÎîĞîßõ³"}; 
const static unsigned char PY_mb_ex_c[]=   {"²ú³É³Ì³Â²Ì²Ü´Ş³şñÒ³Ø³ğêËá¯´Ó´Ë´¦³µ³¡´«³§²Å´Î²é"}; 
const static unsigned char PY_mb_ex_ca[]=   {"²Á²ğàêíå"}; 
const static unsigned char PY_mb_ex_cai[]=   {"²É²Å²Ä²Ë²Æ²Ã²Ê²Â²Ç²È²Ì"}; 
const static unsigned char PY_mb_ex_can[]=  {"²Î²Ğ²Ï²Ó²Í²Ñ²Òåîæîè²ôÓ÷õ"}; 
const static unsigned char PY_mb_ex_cang[]=  {"²Ø²Ö²Ô²Õ²×"}; 
const static unsigned char PY_mb_ex_cao[]=   {"²İ²Û²Ù²Ú²ÜàĞäîó©ô½"}; 
const static unsigned char PY_mb_ex_ce[]=   {"²â²ß²à²á²Şâü"}; 
const static unsigned char PY_mb_ex_cen[]=  {"²Îá¯ä¹"}; 
const static unsigned char PY_mb_ex_ceng[]= {"²ãÔø²äàá"}; 
const static unsigned char PY_mb_ex_ch[]=  {"²î³¤³£²ı³¬³Â³Ì³Ç³ÖÖØ³ö³õ"}; 
const static unsigned char PY_mb_ex_cha[]=  {"²é²î²å²ì²è²æ²ç²ê²ë²í²ïÉ²ÔûàêâªâÇãâæ±è¾é«é¶éßïÊïïñÃ"}; 
const static unsigned char PY_mb_ex_chai[]=  {"²î²ñ²ğ²òÙ­îÎğûò²"}; 
const static unsigned char PY_mb_ex_chan[]= {"²úµ¥²ù²û²ó²ô²õ²ö²÷²ø²üÙæÚÆİÛâÜâãäıå¤åîåñæ¿æöêèìøïâó¸õğ"}; 
const static unsigned char PY_mb_ex_chang[]= {"³¤³£³¡³§³ª³¦²ı³«³¥³©²ş³¢³¨ÉÑÌÈØöÛËÜÉİÅáäâêã®ãÑæ½æÏêÆë©öğ"}; 
const static unsigned char PY_mb_ex_chao[]=  {"³¯³¬³±³²³­³®³°³³³´´Â½Ëâ÷êËìÌñé"}; 
const static unsigned char PY_mb_ex_che[]=   {"³µ³¹³ß³·³¶³¸³ºÛåíº"}; 
const static unsigned char PY_mb_ex_chen[]=  {"³Â³Á³Æ³Ä³¾³¼³¿Éò³»³½³À³ÃØ÷ÚÈÚßŞÓàÁå·è¡é´í×ö³"}; 
const static unsigned char PY_mb_ex_cheng[]= {"³É³Ì³Æ³Ç³Ğ³Ë³ÊÊ¢³Å³Ï³È³Í³Î³Ñ³Ò³ÓØ©ÛôàáèÇèßëóîªîñîõñÎòÉõ¨"}; 
const static unsigned char PY_mb_ex_chi[]=		{"³Ö³ß³İ³Ô³à³Ø³Ù³á³â³Ü³Õ³×³Ú³Û³Ş³ãÙÑÛæÜ¯Üİß³ßêà´àÍáÜâÁæÊë·í÷ğ·ñ¡ñİò¿ó¤ó×óøôùõØ÷Î"}; 
const static unsigned char PY_mb_ex_chong[]= {"ÖÖ³æÖØ³ä³å³çÓ¿³èÜûâçã¿ï¥ô©ô¾"}; 
const static unsigned char PY_mb_ex_chou[]=  {"³é³ğ³ô³ê³ë³ì³í³î³ï³ñ³ò³óÙ±àüã°ñ¬öÅ"}; 
const static unsigned char PY_mb_ex_chu[]=  {"³ö´¦³ı³õ´¡´¥³ş³ú´¢Ğó³÷³ø³ù³û³ü´£´¤Ø¡Û»âğãÀç©èÆèúéËñÒòÜõé÷íåø"}; 
const static unsigned char PY_mb_ex_chuai[]= {"´§Şõà¨àÜëúõß"}; 
const static unsigned char PY_mb_ex_chuan[]= {"´«´¬´©´®´¨´ª´­â¶å×çİë°îËô­"}; 
const static unsigned char PY_mb_ex_chuang[]={"´²´´´°´³´¯´±âë"}; 
const static unsigned char PY_mb_ex_chui[]= {"´µ´¹´¸×µ´¶´·Úïé¢é³"}; 
const static unsigned char PY_mb_ex_chun[]=  {"´º´¿´¼´»´½´¾´Àİ»ğÈòí"}; 
const static unsigned char PY_mb_ex_chuo[]= {"´Á´Âà¨ê¡õÖöº"}; 
const static unsigned char PY_mb_ex_ci[]=  {"´Ë´Î´Ì´Å´Æ´Ê´Ä´Ã´Ç´È´É´ÍËÅ×ÈÜëßÚìôğËôÙ"}; 
const static unsigned char PY_mb_ex_cong[]=  {"´Ó´Ô´Ï´Ğ´Ñ´ÒÜÊäÈæõçıè®"}; 
const static unsigned char PY_mb_ex_cou[]=  {"´Õé¨ê£ëí"}; 
const static unsigned char PY_mb_ex_cu[]=   {"´Ö´Ù´×´Ø×äİıáŞâ§éãõ¡õ¾õí"}; 
const static unsigned char PY_mb_ex_cuan[]=  {"´Û´Ú´ÜÔÜÙàß¥ìàïé"}; 
const static unsigned char PY_mb_ex_cui[]=  {"´ß´à´ã´â´İË¥´Ş´á´äİÍßıã²è­éÁë¥ö¿"}; 
const static unsigned char PY_mb_ex_cun[]=  {"´æ´å´ç¶×ââñå"}; 
const static unsigned char PY_mb_ex_cuo[]=  {"´í´ë´é´è´ê´ìØÈáÏëâï±ïóğîğûõºõã"}; 
const static unsigned char PY_mb_ex_d[]=    {"µÄ´óµØ¶Å¶­¶Î´÷µË¶¡ñ¼µÒµÔµóµçµ³¶¨µÈµ±¶Óµã´úµÀ"}; 
const static unsigned char PY_mb_ex_da[]=    {"´ó´ò´ï´ğ´îËş´ñŞÇßÕàªâòæ§í³ğãñ×óÎ÷°÷²"}; 
const static unsigned char PY_mb_ex_dai[]=  {"´ó´ú´ø´ı´ü´÷´ô´õ´ö´ù´û´şµ¡Ü¤ß°ß¾á·åÊææçªçé÷ì"}; 
const static unsigned char PY_mb_ex_dan[]=  {"µ¥µ«µ¯µ£Ê¯µ°µ­µ¨µªµ¤µ©µ¢µ¦µ§µ¬µ®ÙÙİÌà¢å£ééêæíñğãğ÷ñõóì"}; 
const static unsigned char PY_mb_ex_dang[]=  {"µ³µ±µµµ²µ´ÚÔÛÊİĞå´í¸ñÉ"}; 
const static unsigned char PY_mb_ex_dao[]=   {"µ½µÀµ¼µ¶µ¹µ¾µºµ·µÁµ¸µ»µ¿ß¶àüâáë®ìâôî"}; 
const static unsigned char PY_mb_ex_de[]=   {"µÄµÃµØµÂµ×ï½"}; 
const static unsigned char PY_mb_ex_deng[]= {"µÈµÆµÇµË³ÎµÅµÉµÊàâáØê­íãïëô£"}; 
const static unsigned char PY_mb_ex_di[]=   {"µÄµØµÚµÍµĞµ×ÌáµÛµÖµÎµÜµİµÌµÏµÑµÒµÓµÔµÕµÙµŞØµÙáÚ®ÚĞÛ¡Ûæİ¶àÖæ·ç°èÜé¦êëíÆíÚíûïáôÆ÷¾"}; 
const static unsigned char PY_mb_ex_dia[]=  {"àÇ"}; 
const static unsigned char PY_mb_ex_dian[]=  {"µçµãµæµäµêµßµíµàµáµâµåµèµéµëµìµîÚçÛãáÛçèîäñ°ñ²ô¡õÚ"}; 
const static unsigned char PY_mb_ex_diao[]=  {"µ÷µôµõÄñµïµğµñµòµóµöîöï¢õõöô"}; 
const static unsigned char PY_mb_ex_die[]=  {"µüµøµùµúµûµıµşÛìÜ¦Şéà©ëºğ¬ñóõÚõŞöø"}; 
const static unsigned char PY_mb_ex_ding[]=  {"¶¨¶¥¶¤¶¡¶©¶¢¶£¶¦¶§Øêà¤çàëëíÖî®îúğÛñôôú"}; 
const static unsigned char PY_mb_ex_diu[]=  {"¶ªîû"}; 
const static unsigned char PY_mb_ex_dong[]=  {"¶¯¶«¶¬¶®¶´¶³¶­¶°¶±¶²ÛíßËá´á¼ë±ëËëØíÏğ´"}; 
const static unsigned char PY_mb_ex_dou[]=  {"¶·¶¼¶¹¶Á¶µ¶¶¶¸¶º¶»İúñ¼ò½óû"}; 
const static unsigned char PY_mb_ex_du[]=  {"¶È¶¼¶¾¶À¶Á¶É¶Å¶Â¶Æ¶Ù¶½¶¿¶Ã¶Ä¶Ç¶ÊÜ¶à½äÂèüë¹ó¼óÆ÷Ç÷ò"}; 
const static unsigned char PY_mb_ex_duan[]=  {"¶Ï¶Ë¶Î¶Ì¶Í¶Ğé²ìÑóı"}; 
const static unsigned char PY_mb_ex_dui[]=   {"¶Ô¶Ó¶Ñ¶Ø¶Òí¡í­íÔ"}; 
const static unsigned char PY_mb_ex_dun[]=  {"¶Ü¶Ö¶Ù¶×¶Ø¶Õ¶Ú¶Û¶İãçìÀí»íâíïïæõ»"}; 
const static unsigned char PY_mb_ex_duo[]=  {"¶à¶È¶á¶ä¶Ş¶ß¶â¶ã¶å¶æ¶ç¶è¶éÍÔßÍßáãõç¶èŞîìñÖõâ"}; 
const static unsigned char PY_mb_ex_e[]=   {"¶ñ¶î¶÷¶í¶ú¶ü¶ê¶ö¶ë¶ì¶ï¶ğ¶ò¶ó¶ô¶õØ¬ÚÌÛÑÜÃİ­İàßÀãµåíæ¹éîëñï°ïÉğÊò¦öù"}; 
const static unsigned char PY_mb_ex_ei[]=   {"ÚÀ"}; 
const static unsigned char PY_mb_ex_en[]=   {"¶÷İìŞô"}; 
const static unsigned char PY_mb_ex_er[]=   {"¶ø¶ş¶û¶ù¶ú¶ü¶ı·¡Ù¦åÇçíîïğ¹öÜ"}; 
const static unsigned char PY_mb_ex_f[]=    {"·Ö·¢·¶·®·½·ë·´·Å¸´·ç·À·Ç·â¸»·­"}; 
const static unsigned char PY_mb_ex_fa[]=   {"·¢·¨·§·¦·¥·£·¤·©ÛÒíÀ"}; 
const static unsigned char PY_mb_ex_fan[]=  {"·´·­·¶·¸·¹·±·º·¬·²·³·µ·ª·«·®·¯·°··Ş¬ŞÀá¦èóìÜî²õì"}; 
const static unsigned char PY_mb_ex_fang[]=  {"·½·Å·À·Ã·¿·Ä·Â·Á·¼·¾·»ÚúáİèÊîÕô³öĞ"}; 
const static unsigned char PY_mb_ex_fei[]=  {"·Ç·Ê·É·Ñ·Ï·Î·Ğ·Æ·Ë·È·Ì·ÍÜÀáôã­äÇåúç³é¼ëèì³ìéïĞğòòãóõôäö­öî"}; 
const static unsigned char PY_mb_ex_fen[]=  {"·Ö·Û·Ü·İ·à·×·Ò·ß·Ó·Ô·Õ·Ø·Ù·Ú·ŞÙÇå¯çãèûö÷÷÷"}; 
const static unsigned char PY_mb_ex_feng[]=  {"·ç·â·ä·á·ì·å·æ·è·î·ã·é·ê·ë·í·ïÙºÛºİ×ßôããí¿"}; 
const static unsigned char PY_mb_ex_fo[]=   {"·ğ"}; 
const static unsigned char PY_mb_ex_fou[]=   {"·ñó¾"}; 
const static unsigned char PY_mb_ex_fu[]=   {"¸´·ş¸±¸®·ò¸º¸»¸½¸£·ü·û·ù¸¯¸¡¸¨¸¶¸¹¸¾·õ¸²·ö·ø¸µ·ğ¸¿¸¸¸¥¸¦·ô·ú·ó·÷·ı¸¢¸¤¸§¸©¸ª¸«¸¬¸­¸°¸³¸·¸¼¸ÀÙëÙìÛ®Ü½ÜÀÜŞÜòİ³İÊŞÔß»á¥âöäæåõæÚæâç¦ç¨èõêçìğíÉíêíëî·ïûğ¥ò¶òİòğòóôïõÃõÆöÖöû"}; 
const static unsigned char PY_mb_ex_g[]=   {"¸ö¹ú¹¤¹ı¸ß¹ù¸ê¸ğ¹¢¹®¹¨¸Ê¸É¹Å¼Ö¹È¹Ë¹Ø¹Ü¸÷¸ï¹«¸ù¸ü¸Ä¹â¹ã¹û¸ñ¹²¸ã"}; 
const static unsigned char PY_mb_ex_ga[]=   {"¼Ğ¸Á¸Â¸ìŞÎæØæÙê¸îÅ"}; 
const static unsigned char PY_mb_ex_gai[]=   {"¸Ä¸Ã¸Ç¸Å¸Æ¸È½æØ¤ÚëÛòê®êà"}; 
const static unsigned char PY_mb_ex_gan[]=  {"¸É¸Ë¸Ğ¸Ò¸Ï¸Ê¸Î¸Ñ¸Ì¸Í¸ÓÇ¬ÛáÜÕŞÏß¦ãïäÆä÷ç¤éÏêºí·ğáôû"}; 
const static unsigned char PY_mb_ex_gang[]=  {"¸Õ¸Ö¸×¸Ù¸Ú¸Û¸Ü¸Ô¸Ø¿¸í°î¸óà"}; 
const static unsigned char PY_mb_ex_gao[]=   {"¸ß¸ã¸æ¸å¸à¸İ¸Ş¸á¸â¸äØºÚ¾Û¬Ş»çÉéÀéÂê½ï¯"}; 
const static unsigned char PY_mb_ex_ge[]=   {"¸ö¸÷¸ïºÏ¸ñ¸î¸è¸ô¸Ç¸ç¸ğ¸õ¸ó¸ê¸é¸ë¸ì¸í¸ò¿©ÒÙØªØîÛÁÛÙÜªàÃæüë¡ëõíÑïÓñËò¢ò´ô´÷À"}; 
const static unsigned char PY_mb_ex_gei[]=  {"¸ø"}; 
const static unsigned char PY_mb_ex_gen[]=  {"¸ù¸úØ¨İ¢ßçôŞ"}; 
const static unsigned char PY_mb_ex_geng[]= {"¸ü¸û¾±¸ı¸ş¹¡¹¢¹£ßìâÙç®öá"}; 
const static unsigned char PY_mb_ex_gong[]= {"¹¤¹«¹²¹©¹¦¹¥ºì¹®¹±¹¯¹¬¹§¹¨¹ª¹­¹°çîëÅò¼ö¡"}; 
const static unsigned char PY_mb_ex_gou[]=  {"¹»¹¹¹µ¾ä¹·¹³¹´¹º¹¶¹¸ØşÚ¸á¸åÜæÅçÃèÛêíì°óÑóô÷¸"}; 
const static unsigned char PY_mb_ex_gu[]=  {"¹Ä¹Ì¹Å¹Ç¹Ê¹Ë¹É¹È¹À¹Í¹Â¹Ã¼Ö¹¼¹½¹¾¹¿¹Á¹ÆØÅÚ¬İÔßÉáÄãéèôéïêôêöëûì±î­î¹îÜïÀğ³ğÀğóòÁôşõıöñ÷½"}; 
const static unsigned char PY_mb_ex_gua[]=  {"¹Ò¹Î¹ÏÀ¨¹Ğ¹Ñ¹ÓØÔÚ´ßÉèéëÒğ»"}; 
const static unsigned char PY_mb_ex_guai[]=  {"¹Ö¹Ô¹ÕŞâ"}; 
const static unsigned char PY_mb_ex_guan[]= {"¹Ø¹Ü¹Û¹Ù¹à¹á¹ß¹Ú¹İ¹Ş¹×ÂÚÙÄİ¸ŞèäÊîÂğÙñæ÷¤"}; 
const static unsigned char PY_mb_ex_guang[]={"¹â¹ã¹äßÛáîèæë×"}; 
const static unsigned char PY_mb_ex_gui[]=  {"¹æ¹ó¹é¹è¹í¹ì¹ê¹ğ¹å¹ç¹ë¹î¹ï¹ñ¹ò¹ô¿şÈ²ØĞØÛâÑå³æ£èíêÁêĞğ§óşöÙ÷¬"}; 
const static unsigned char PY_mb_ex_gun[]=  {"¹ö¹õ¹÷ÙòçµíŞöç"}; 
const static unsigned char PY_mb_ex_guo[]=  {"¹ú¹ı¹û¹ø¹ùÎĞ¹üÙåÛöŞâßÃàşáÆâ£é¤ë½ñøòäòå"}; 
const static unsigned char PY_mb_ex_h[]=   {"ºÍ»áºóºÎ»ÆºúºéºîºØ»ôìèº«»ªºÂ»¯ºÃºÏ»¹»îºÜ»ØºÅºÁ»¨»òº£»°ºì»ğº¦"}; 
const static unsigned char PY_mb_ex_ha[]=   {"¹ş¸òÏºîş"}; 
const static unsigned char PY_mb_ex_hai[]=   {"»¹º£º¦º¢º¡º¤º¥º§ºÙ¿ÈàËëÜõ°"}; 
const static unsigned char PY_mb_ex_han[]=   {"º¬º¸ºµº°ººº®º¹º¯º«º¨º©ºªº­º±º²º³º´º¶º·ÚõİÕŞşãÛå«êÏìÊñüò¥òÀ÷ı"}; 
const static unsigned char PY_mb_ex_hang[]= {"ĞĞº½º»º¼¿ÔÏïãìç¬çññş"}; 
const static unsigned char PY_mb_ex_hao[]=   {"ºÃºÅºÁºÄºÀºÂºÆ¸äº¾º¿ºÑİïŞ¶àÆàãå©å°ê»ğ©ò«òº"}; 
const static unsigned char PY_mb_ex_he[]=   {"ºÍºÏºÓºÎºËºÕºÉºÖºÈºØºÇºÌÏÅºĞºÊºÑºÒºÔº×Ú­ÛÀÛÖàÀãØæüêÂîÁò¢òÂôç"}; 
const static unsigned char PY_mb_ex_hei[]=   {"ºÚºÙàË"}; 
const static unsigned char PY_mb_ex_hen[]=  {"ºÜºİºÛºŞ"}; 
const static unsigned char PY_mb_ex_heng[]= {"ºáºâºãºßºàŞ¿ĞĞçñèì"}; 
const static unsigned char PY_mb_ex_hong[]=  {"ºìºéºäºæºåºçºèºêºëÙäÙêÚ§İ¦Ş®Ş°ãÈãü"}; 
const static unsigned char PY_mb_ex_hou[]=  {"ºóºòºñºîºíºïºğÜ©ááåËğúóóô×ö×÷¿"}; 
const static unsigned char PY_mb_ex_hu[]=  {"ºÍ»¤»¥ºşºô»§»¡ºõºúºı»¢ºöºËº÷Ï·ºøºùºûºü»£»¦Ùüßüàñá²â©âïã±ä°äïçúéÎéõì²ìÃìÎìæìèìïğ­ğÀğÉğ×óËõ­õú"}; 
const static unsigned char PY_mb_ex_hua[]=  {"»¯»¨»°»®»¬»ª»­»©»«»íæèèëí¹îü"}; 
const static unsigned char PY_mb_ex_huai[]=  {"»µ»³»®»´»±»²õ×"}; 
const static unsigned char PY_mb_ex_huan[]= {"»¹»·»»»¶»º»¼»Ã»À»¸»½»¾»¿»Á»ÂÛ¨Û¼İÈß§à÷âµä¡ä½äñå¾åÕçÙïÌöé÷ß"}; 
const static unsigned char PY_mb_ex_huang[]= {"»Æ»É»Ä»Ê»Å»È»Ç»Ë»Ì»Í»Î»Ï»Ğ»ÑÚòáåäÒäêåØè«ëÁñ¥ó¨óòöü"}; 
const static unsigned char PY_mb_ex_hui[]=  {"»á»Ø»Ò»Ó»Ô»ã»Ù»Û»Ö»æ»İ»ÕÀ£¶é»²»×»Ú»Ü»Ş»ß»à»â»ä»åÚ¶ÜîÜöŞ¥ßÔßÜà¹ãÄä§ä«åççÀçõèíêÍí£ò³ó³÷â"}; 
const static unsigned char PY_mb_ex_hun[]=  {"»ì»ë»ç»è»é»êÚ»âÆãÔäã"}; 
const static unsigned char PY_mb_ex_huo[]=  {"ºÍ»î»ò»ğ»õ»ñ»ï»ô»í»ó»öØåŞ½ß«àëâ·îØïÁïìñëó¶"}; 
const static unsigned char PY_mb_ex_j[]=   {"¼¶¼Ö½­½¯½ª½¹½ğ½ù½ú¾Í½ø¼Ó¾­¼Ò½á½×¼ä¼ş¼°½â½Ï»ú¾ü½Ó½Ì¼«¾Å¼û"}; 
const static unsigned char PY_mb_ex_ji[]=   {"¼¶¼°»ú¼«¼¸»ı¸ø»ùÆä¼Ç¼ºÆÚÏµ¸ï¼Æ¼¯¼´¼Ê¼¾¼¤¼Ã¼¼»÷¼Ì¼±¼Á¼È¼Í¼Ä¼·¼¦¼£¼¨ÆëÆæ¼ª¼¹¼­¼®¼²¼¡¼¬»û»ø»ü»ş¼¢¼¥¼§¼©¼³¼µ¼»¼½¼¿¼À¼Â¼Å¼É¼Ë½åØ¢Ø½ØÀØŞÙ¥ÙÊÚµÛÔÜ¸ÜÁÜùİğŞªŞáß´ßÒßâßóá§áÕä©åìæ÷çÜçáé®éêêªê«êåêéê÷ì´í¶î¿ïúğ¢ñ¤ò±óÅóÇôßõÒõÕö«öİöê÷Ù÷ä"}; 
const static unsigned char PY_mb_ex_jia[]=  {"¼Ó¼Ò¼Ü¼Û¼×¼Ğ¼Ù¼Ø¼Ö¼Ú¼İ¼Î¼Ï¼Ñ¼Ô¼Õ¼ŞÇÑĞ®ØÅÙ¤Û£İçáµä¤åÈçìê©ëÎí¢îòïØğèğıñÊòÌóÕôÂõÊ"}; 
const static unsigned char PY_mb_ex_jian[]=  {"¼ä¼ş¼û½¨¼á¼õ¼ì¼ù¼â¼ò¼î¼ô¼è½¥¼ç¼ü½¡¼í¼ø½£¼ß¼à¼æÇ³¼é¼ı¼ë½¢¼ó¼ã¼å¼ê¼ï¼ğ¼ñ¼ö¼÷¼ú½¤½¦½§ÙÔÚÉÚÙİÑİóŞöàîäÕå¿åÀçÌèÅé¥ê§ê¯êğêùë¦ëìíúïµğÏñĞóÈôåõÂõİöä÷µ"}; 
const static unsigned char PY_mb_ex_jiang[]= {"½«½µ½²Ç¿½­½¬½¯½±½®½©½ª½°½³½´Üüä®ç­çÖêñíäñğôİôø"}; 
const static unsigned char PY_mb_ex_jiao[]= {"½Ï½Ì½»½Ç½Ğ½Å½º½½½¹¾õĞ£½¶½Á½Í½¼½Â½Ñ½·½¸½¾½¿½À½Ã½Ä½Æ½È½É½Ê½Ë½ÎÙ®ÙÕÜ´ÜúŞØàİá½áèæ¯ë¸ğ¨ğÔòÔõ´õÓöŞ"}; 
const static unsigned char PY_mb_ex_jie[]=  {"½á½×½â½Ó½Ú¼Ò½ç½Ø½é½è¼Û½ì½Ö½Ò½à½Ü½ß½Ô½Õ½Ù½Û½İ½Ş½ã½ä½å½æ½ê½ë¿¬ÙÊÚ¦ÚµŞ×à®àµæ¼æİèîíÙğÜò¡ò»ôÉöÚ÷º"}; 
const static unsigned char PY_mb_ex_jin[]= {"½ø½ğ½ü½ô½ï½ñ¾¡½ö¾¢½ş½û½ò½î½õ½ú½í½ó½÷½ù½ıÚáİ£İÀàäâËâÛæ¡çÆèªéÈêáêîñÆñæ"}; 
const static unsigned char PY_mb_ex_jing[]= {"¾­¾«¾©¾¶¾®¾²¾¹¾§¾»¾³¾µ¾°¾¯¾¢¾¥¾´¾ª¾¦¾º¾£¾¤¾¨¾¬¾·¾¸ØÙÙÓÚåİ¼â°ã½ãşåÉåòæºëÂëÖëæìºö¦"}; 
const static unsigned char PY_mb_ex_jiong[]= {"¾¼¾½åÄêÁìç"}; 
const static unsigned char PY_mb_ex_jiu[]=  {"¾Í¾Å¾É¾¿¾Ã¾È¾Æ¾À¾¾¾Á¾Â¾Ä¾Ç¾Ê¾Ë¾Ì¾ÎÙÖà±ãÎèÑèêğ¯ğÕôñ÷İ"}; 
const static unsigned char PY_mb_ex_ju[]=   {"¾ß¾İ¾Ö¾Ù³µ¾ä¾Û¾à¾Ş¾Ó¾â¾ç¾Ø¾Ü¹ñ½Û¾Ï¾Ğ¾Ñ¾Ò¾Ô¾Õ¾×¾Ú¾á¾ã¾å¾æÙÆÚªÜÄÜÚÜìŞäåáåğè¢èÛé§é°é·éÙêøì«îÒï¸ñÀñÕôòõ¶õáö´öÂöÄ÷¶"}; 
const static unsigned char PY_mb_ex_juan[]=  {"¾í¾è¾é¾ê¾ë¾ì¾î¿¡Û²áúä¸èğîÃïÃïÔöÁ"}; 
const static unsigned char PY_mb_ex_jue[]=   {"¾ö¾õ¾ø½Ç½Å¾ò½À¾ï¾ğ¾ñ¾ó¾ô¾÷ØÊØãÚÜÛÇŞ§àµàÙàåáÈâ±æŞçåèöéÓìßïãõêõû¾Ş¶î"}; 
const static unsigned char PY_mb_ex_jun[]=   {"¾ü¾ù¾ú¾ı¹ê¾û¾ş¿¡¿¢¿£¿¤¿¥ŞÜñäóŞ÷å"}; 
const static unsigned char PY_mb_ex_k[]=   {"¿É¿ª¿´Øá¿Ë¿×Ú÷¿Ü¿Æ¿Ú¿Õ¿à¿ì¿¼¿Ì¿¿¿ö¿éÀ§¿í¿ó¿¹¿¨"}; 
const static unsigned char PY_mb_ex_ka[]=   {"¿¨¿¦¿§¿©ØûßÇëÌ"}; 
const static unsigned char PY_mb_ex_kai[]=   {"¿ª¿­¿«¿¬¿®ØÜÛîİÜâéâıîøï´ïÇ"}; 
const static unsigned char PY_mb_ex_kan[]=  {"¿´¿¯¿²¼÷¿°¿±¿³Ç¶Ù©İ¨ãÛê¬íèî«"}; 
const static unsigned char PY_mb_ex_kang[]= {"¿¹¿µ¿»¿¶¿·¿¸¿ºØøãÊîÖ"}; 
const static unsigned char PY_mb_ex_kao[]=  {"¿¼¿¿¿½¿¾åêèàêûîí"}; 
const static unsigned char PY_mb_ex_ke[]=  {"¿É¿Ë¿Æ¿Ì¿Í¿Ç¿Å¿Ã¿ÂºÇ¿À¿Á¿Ä¿È¿Ê¿Îà¾á³ã¡äÛæìç¼çæéğë´î§îİï¾ïığâñ½ò¤òò÷Á"}; 
const static unsigned char PY_mb_ex_ken[]= {"¿Ï¿Ğ¿Ñ¿ÒñÌ"}; 
const static unsigned char PY_mb_ex_keng[]= {"¿Ó¿Ôï¬"}; 
const static unsigned char PY_mb_ex_kong[]= {"¿×¿Õ¿Ø¿ÖÙÅáÇóí"}; 
const static unsigned char PY_mb_ex_kou[]=  {"¿Ú¿Û¿Ù¿ÜÜÒŞ¢ßµíîóØ"}; 
const static unsigned char PY_mb_ex_ku[]=  {"¿à¿â¿İ¿á¿Ş¿ß¿ãØÚÜ¥à·ç«÷¼"}; 
const static unsigned char PY_mb_ex_kua[]=  {"¿ç¿ä¿å¿æ¿èÙ¨"}; 
const static unsigned char PY_mb_ex_kuai[]=  {"»á¿ì¿é¿ê¿ëØáÛ¦ßàáöä«ëÚ"}; 
const static unsigned char PY_mb_ex_kuan[]= {"¿í¿î÷Å"}; 
const static unsigned char PY_mb_ex_kuang[]= {"¿ö¿ó¿ñ¿ò¿ï¿ğ¿ô¿õÚ²Ú¿Ú÷ÛÛŞÅßÑæşêÜ"}; 
const static unsigned char PY_mb_ex_kui[]=  {"¿üÀ£À¡¿÷¿ø¿ù¿ú¿û¿ı¿şÀ¢Ø¸ØÑÙçÚóİŞŞñà­à°ã¦ã´åÓêÒî¥ñùòñóñõÍ"}; 
const static unsigned char PY_mb_ex_kun[]=  {"À§À¥À¤À¦ã§ãÍçûï¿õ«öï÷Õ"}; 
const static unsigned char PY_mb_ex_kuo[]=  {"À©À¨ÊÊÀ«ÀªèéòÒ"}; 
const static unsigned char PY_mb_ex_l[]=   {"ÁËÀ´ÀµÂŞÂÀÁõÂ³ÀîÁÖÀ×ÁÎÂ¬ÁºÀ÷Á®Â½ÁúÀèÂ¹Á¿ÀíÂ·ÀïÂÛÁ½ÀûÁìÁ÷Á¦Á¢ÁÏÁ¬ÀÏÁùÂÖÀàÁĞ"}; 
const static unsigned char PY_mb_ex_la[]=   {"À­ÂäÀ²À¯À°À¶À¬À®À±Øİååê¹íÇğø"}; 
const static unsigned char PY_mb_ex_lai[]=   {"À´ÀµÀ³áÁáâäµäşêãíùïªñ®ô¥"}; 
const static unsigned char PY_mb_ex_lan[]=  {"À¼ÀÃÀ¶ÀÀÀ¸À·À¹ÀºÀ»À½À¾À¿ÀÁÀÂÀÄá°äíé­ìµî½ïçñÜ"}; 
const static unsigned char PY_mb_ex_lang[]= {"ÀËÀÊÀÉÀÇÀÅÀÆÀÈİ¹İõà¥ãÏï¶ïüòë"}; 
const static unsigned char PY_mb_ex_lao[]= {"ÀÏÀÍÂäÀÎÀÔÀÌÂçÀĞÀÑÀÒÀÓÁÊßëáÀèáîîï©ğìñìõ²"}; 
const static unsigned char PY_mb_ex_le[]=   {"ÁËÀÖÀÕÀßØìß·ãî÷¦"}; 
const static unsigned char PY_mb_ex_lei[]=  {"ÀàÀ×ÀÛÀÕÀİÀáÀØÀÙÀÚÀÜÀŞÀßÙúÚ³àÏæĞçĞéÛñçõª"}; 
const static unsigned char PY_mb_ex_leng[]=  {"ÀäÀâÀãÜ¨ã¶"}; 
const static unsigned char PY_mb_ex_li[]=  {"ÀíÀïÀûÁ¦Á¢ÀëÀıÀúÁ£ÀåÀñÀîÁ¥ÀèÁ§ÀøÀçÀæÀöÀ÷ÀéÀêÀìÀğÀòÀóÀôÀõÀùÀüÀşÁ¡Á¤Á¨ØªÙ³ÙµÛªÛŞÜÂİ°İñŞ¼ß¿à¦à¬áûã¦äàå¢åÎæ²æËæêçÊèÀèİéöìåíÂîºî¾ï®ğ¿ğİğßòÃòÛó»óÒóöôÏõ·õÈö¨öâ÷¯÷ó"}; 
const static unsigned char PY_mb_ex_lia[]=  {"Á©"}; 
const static unsigned char PY_mb_ex_lian[]=  {"Á¬ÁªÁ·Á¶Á³Á´Á«Á­Á®Á¯Á°Á±Á²ÁµİüŞÆäòå¥çöé¬éçì¡ñÍñÏó¹öãÁ¢°¸"}; 
const static unsigned char PY_mb_ex_liang[]= {"Á¿Á½Á¸Á¼ÁÁÁºÁ¹Á¾Á»ÁÀÁÂÜ®İ¹é£õÔö¦÷Ë"}; 
const static unsigned char PY_mb_ex_liao[]=  {"ÁËÁÏÁÆÁÉÁÅÁÃÁÄÁÇÁÈÁÊÁÌÁÍÁÎŞ¤ŞÍàÚâ²å¼çÔîÉğÓ"}; 
const static unsigned char PY_mb_ex_lie[]=  {"ÁĞÁÑÁÒÁÓÁÔÙıÛøŞæßÖä£ôóõñ÷à"}; 
const static unsigned char PY_mb_ex_lin[]=  {"ÁÖÁ×ÁÙÁÚÁÜ÷ëÁÕÁØÁÛÁİÁŞÁßİşßøá×âŞãÁåàéİê¥ì¢î¬ôÔõï"}; 
const static unsigned char PY_mb_ex_ling[]= {"ÁìÁíÁãÁîÁéÁëÁåÁäÁèÀâÁêÁàÁáÁâÁæÁçÛ¹ÜßßÊàòãöç±èÚèùê²ñöòÈôáöì"}; 
const static unsigned char PY_mb_ex_liu[]=  {"Á÷ÁùÁôÁõÁòÁøÂ½ÁóÁöÁïÁğÁñÂµä¯åŞæòç¸ì¼ìÖï³ïÖğÒöÌ"}; 
const static unsigned char PY_mb_ex_lo[]=  {"¿©"}; 
const static unsigned char PY_mb_ex_long[]=  {"ÁúÂ¢ÅªÁıÂ¡ÁûÁüÁşÂ£Â¤ÛâÜ×ãñççèĞëÊíÃñª"}; 
const static unsigned char PY_mb_ex_lou[]=  {"Â©Â¶Â¥Â¦Â§Â¨ÂªÙÍİäà¶áĞïÎğüñïò÷÷Ã"}; 
const static unsigned char PY_mb_ex_lu[]=  {"Â·Â¼Â½Â¯Â¶Â«Â¬Â­Â®Â°Â±Â²Â³Â´ÂµÂ¸Â¹ÂºÂ»Â¾Ûäß£ààãòäËäõåÖè´èÓéÖéñéûê¤ëªëÍïåğµğØóüôµöÔÁùÂÌ"}; 
const static unsigned char PY_mb_ex_luan[]=  {"ÂÑÂÒÂÍÂÎÂÏÂĞÙõæ®èïğ½öÇ"}; 
const static unsigned char PY_mb_ex_lue[]=  {"ÂÔÂÓï²"}; 
const static unsigned char PY_mb_ex_lun[]=  {"ÂÛÂÖÂ×ÂÕÂØÂÙÂÚàğ"}; 
const static unsigned char PY_mb_ex_luo[]=   {"ÂäÂŞÂİÂåÂçÂß¿©ÀÓÂÜÂàÂáÂâÂãÂæÙÀÙùÜıŞÛŞûâ¤ãøäğçóé¡ëáíÑïİñ§öÃ"}; 
const static unsigned char PY_mb_ex_lv[]=   {"ÂÊÂÉÂÃÂÇÂ¿ÂÀÂÁÂÂÂÄÂÅÂÆÂÈÂËÂÌŞÛãÌéµëöïùñÚÙÍ"}; 
const static unsigned char PY_mb_ex_m[]=   {"ÃæÃüÂíÃÏÄÂÃ·ÃÓÃÇÃñÃ«Ã©ÃçÄªÃ»Ã×Ã÷Ã´Ã¿ÃÅÃûÄ¥ÃÀÃÜÂúÄ¶Ä¿ß¼"}; 
const static unsigned char PY_mb_ex_ma[]=   {"ÂíÃ´ÂéÂğÂèÂîÂïÂëÄ¦Ä¨ÂêÂìßéáïæÖè¿ó¡"}; 
const static unsigned char PY_mb_ex_mai[]=   {"ÂóÂöÂôÂòÂñÂõÛ½İ¤ö²"}; 
const static unsigned char PY_mb_ex_man[]=  {"ÂúÂıÂüÂşÂûÂ÷ÂøÂùÃ¡Ü¬á£çÏì×ïÜò©òı÷©÷´"}; 
const static unsigned char PY_mb_ex_mang[]= {"Ã¦Ã¢Ã¤Ã£Ã¥Ã§ÚøäİíËòş"}; 
const static unsigned char PY_mb_ex_mao[]=   {"Ã«Ã¬Ã°Ã²Ã³Ã±Ã¨Ã©ÃªÃ­Ã®Ã¯ÙóÜâá¹ã÷è£êÄêóë£ì¸í®î¦ó±÷Ö"}; 
const static unsigned char PY_mb_ex_me[]=  {"Ã´÷á"}; 
const static unsigned char PY_mb_ex_mei[]= {"Ã»Ã¿ÃÀÃºÃ¹Ã¸Ã·ÃÃÃ¼ÃµÃ¶Ã½Ã¾ÃÁÃÂÃÄÃÓÃÕİ®áÒâ­ä¼äØé¹ïÑğÌñÇ÷È"}; 
const static unsigned char PY_mb_ex_men[]=  {"ÃÇÃÅÃÆŞÑìËí¯îÍ"}; 
const static unsigned char PY_mb_ex_meng[]=  {"ÃÏÃÍÃÉÃËÃÎÃÈÃÌÃ¥ÃÊÛÂİùŞ«ãÂëüíæòµòìó·ô»ô¿"}; 
const static unsigned char PY_mb_ex_mi[]=  {"Ã×ÃÜÃÔÃÛÃØÃĞÃÑÃÒÃÓÃÕÃÖÃÙÃÚÃİØÂÚ×ŞÂßäà×â¨ãèåµåôëßìòôÍôé÷ã÷ç"}; 
const static unsigned char PY_mb_ex_mian[]=  {"ÃæÃŞÃâÃàÃßÃáÃãÃäÃåãæäÅäÏëïíí"}; 
const static unsigned char PY_mb_ex_miao[]= {"ÃçÃëÃèÃíÃîÃéÃêÃìß÷åãç¿çÑèÂíµíğğÅ"}; 
const static unsigned char PY_mb_ex_mie[]=  {"ÃğÃïØ¿ßãóºóú"}; 
const static unsigned char PY_mb_ex_min[]=  {"ÃñÃôÃòÃóÃõÃöÜåáºãÉãıçÅçäçëíªö¼÷ª"}; 
const static unsigned char PY_mb_ex_ming[]=  {"ÃüÃ÷ÃûÃùÃøÃËÃúÚ¤ÜøäéêÔî¨õ¤"}; 
const static unsigned char PY_mb_ex_miu[]=  {"ÃıçÑ"}; 
const static unsigned char PY_mb_ex_mo[]=  {"ÎŞÄ¥Ã»ÍòÄ©Ä£Ä¤ÃşÄ«Ä¦ÂöÄªÄ¨Ã°Ä¬ºÑºÙÄ¡Ä¢Ä§Ä­Ä®Ä¯Ä°ÚÓÜÔİëâÉæÆéâïÒï÷ñ¢ñòõöõø"}; 
const static unsigned char PY_mb_ex_mou[]=  {"Ä³Ä±Ä²Ù°ßèçÑíøòÖòúöÊ"}; 
const static unsigned char PY_mb_ex_mu[]=   {"Ä¶Ä¿Ä¾Ä¸Ä¹Ä£Ä»ÄÁÄ·ÄÂÀÑÄ²Ä´ÄµÄºÄ¼Ä½ÄÀØïÛéÜÙãåë¤îâ"}; 
const static unsigned char PY_mb_ex_n[]=   {"ÄêÄÜÄÚÄÇÄôÅ©ÄãÄÏÄÑÄØÅ®ÄàÄÄÄÃÄîÅ£ÄşÄÔÕ³ÄáÄÉàÅ"}; 
const static unsigned char PY_mb_ex_na[]=   {"ÄÇÄÏÄÄÄÃÄÉÄØÄÆÄÅÄÈŞàëÇïÕñÄ"}; 
const static unsigned char PY_mb_ex_nai[]=  {"ÄÍÄÄÄÌÄËÄÊÄÎØ¾ÜµİÁèÍ"}; 
const static unsigned char PY_mb_ex_nan[]=  {"ÄÏÄÑÄĞà«àîàïéªëîòïôö"}; 
const static unsigned char PY_mb_ex_nang[]= {"ÄÒß­àìâÎêÙ"}; 
const static unsigned char PY_mb_ex_nao[]=  {"ÄÔÄÖÅ¬ÄÓÄÕÄ×Ø«ÛñßÎâ®è§íĞîóòÍ"}; 
const static unsigned char PY_mb_ex_ne[]=  {"ÄØÄÄÄÅÚ«"}; 
const static unsigned char PY_mb_ex_nei[]=  {"ÄÚÄÇÄÄÄÙ"}; 
const static unsigned char PY_mb_ex_nen[]=  {"ÄÛí¥"}; 
const static unsigned char PY_mb_ex_neng[]= {"ÄÜ"}; 
const static unsigned char PY_mb_ex_ni[]=  {"ÄãÄàÄáÄØÄæÄâÄòÄİÄŞÄßÄäÄåÄçÙ£Ûèâ¥âõêÇì»ìòí«íşîêöò"}; 
const static unsigned char PY_mb_ex_nian[]= {"ÄêÄîÕ³ÄèÄéÄëÄìÄíÄğØ¥Ûşéığ¤öÓöó"}; 
const static unsigned char PY_mb_ex_niang[]={"ÄïÄğ"}; 
const static unsigned char PY_mb_ex_niao[]=  {"ÄòÄñÄçÜàæÕëåôÁ"}; 
const static unsigned char PY_mb_ex_nie[]=  {"ÄøÄöÄùÄóÄôÄõÄ÷Ø¿ÚíŞÁà¿ò¨ô«õæ"}; 
const static unsigned char PY_mb_ex_nin[]=  {"Äúí¥"}; 
const static unsigned char PY_mb_ex_ning[]= {"ÄşÄıÅ¡ÄûÄüÅ¢ØúÜÑßÌå¸ñ÷"}; 
const static unsigned char PY_mb_ex_niu[]=  {"Å£Å¤Å¥Å¦ŞÖáğâîæ¤"}; 
const static unsigned char PY_mb_ex_nong[]= {"Å©ÅªÅ¨Å§Ù¯ßæ"}; 
const static unsigned char PY_mb_ex_nou[]=  {"ññ"}; 
const static unsigned char PY_mb_ex_nu[]=  {"Å¬Å«Å­åóæÀæÛæå"}; 
const static unsigned char PY_mb_ex_nuan[]=  {"Å¯"}; 
const static unsigned char PY_mb_ex_nuo[]=   {"ÅµÄÈÅ²Å³Å´ÙĞŞùßöï»"}; 
const static unsigned char PY_mb_ex_nv[]=  {"Å®í¤îÏô¬"}; 
const static unsigned char PY_mb_ex_nve[]=  {"Å°Å±"}; 
const static unsigned char PY_mb_ex_o[]=  {"Å¶à¸àŞÅ·Å¼Å¸Å¹ÅºÅ»âæñî"}; 
const static unsigned char PY_mb_ex_ou[]=  {"ÇøÅ·Å¼Å¸Å¹ÅºÅ»Å½Ú©âæê±ñî"}; 
const static unsigned char PY_mb_ex_p[]=   {"ÅúÆ½ÅĞÅËÅíÅÓÅáÆÑÅäÆ¬ÅÉÆÆÅÅÆ·Æ¤ÅçÅÌÆ«ÆÀÅÂÅàÆÕÆ¶ÅÜÅÚ"}; 
const static unsigned char PY_mb_ex_pa[]=  {"ÅÂÅÉÅÀÅÁ°Ç°ÒÅ¾Å¿ÅÃİâèËîÙóá"}; 
const static unsigned char PY_mb_ex_pai[]=  {"ÅÉÅÅÅÄÅÆÆÈßßÅÇÅÈÙ½İå"}; 
const static unsigned char PY_mb_ex_pan[]=  {"ÅĞÅÌÅÑ·¬ÅËÅÊÅÍÅÎÅÏÅÖŞÕãİãúñÈñáó´õç"}; 
const static unsigned char PY_mb_ex_pang[]= {"ÅÔ°ò°õÅÒÅÓÅÕÅÖáİäèåÌó¦"}; 
const static unsigned char PY_mb_ex_pao[]=  {"ÅÜÅÚÅÙÅ×ÅİÅØÅÛŞËáóâÒëãğå"}; 
const static unsigned char PY_mb_ex_pei[]=  {"ÅäÅàÅãÅßÅŞÅáÅâÅåÅæàÎàúì·ïÂõ¬ö¬"}; 
const static unsigned char PY_mb_ex_pen[]=  {"ÅçÅèäÔ"}; 
const static unsigned char PY_mb_ex_peng[]= {"ÅöÅïÅîÅóÅõÅòÅéÅêÅëÅìÅíÅğÅñÅôÜ¡àØâñó²"}; 
const static unsigned char PY_mb_ex_pi[]=  {"ÅúÆ¤»µ·ñ±ÙÅ÷Æ¢Æ£ÅøÅùÅûÅüÅıÅşÆ¡Æ¥Æ¦Æ§Æ¨Æ©Ø§ØòÚéÚğÚüÛ¯ÛÜÛıÜ±ÜÅß¨ßÁàèâÏäÄæÇç¢èÁê¶î¢î¼îëñ±ñÔñâò·òçõù"}; 
const static unsigned char PY_mb_ex_pian[]=  {"Æ¬Æ«±ãÆªÆ­±âÚÒæéçÂêúëİôæõä"}; 
const static unsigned char PY_mb_ex_piao[]=  {"Æ±Æ¯ÆÓÆ®Æ°Øâİ³àÑæÎæôçÎéèî©óª"}; 
const static unsigned char PY_mb_ex_pie[]=  {"Æ²Æ³Ø¯ÜÖë­"}; 
const static unsigned char PY_mb_ex_pin[]=  {"Æ·Æ¶ÆµÆ´Æ»Æ¸ŞÕæ°æÉé¯êòò­"}; 
const static unsigned char PY_mb_ex_ping[]= {"Æ½ÆÀÆ¿Æ¾Æ»·ëÆ¹ÆºÆ¼ÆÁÙ·æ³èÒöÒÚ¢"}; 
const static unsigned char PY_mb_ex_po[]=  {"ÆÆÆÈÆÂÆÃ·±ÆÓ²´ÆÄÆÅÆÇÆÉØÏÚéÛ¶ãøçêê·îÇîŞîàğ«óÍ"}; 
const static unsigned char PY_mb_ex_pou[]=  {"ÆÊÙöŞå"}; 
const static unsigned char PY_mb_ex_pu[]=   {"ÆÕÆ×ÆËÆÒÆÌ±©ÆÏÆÓÆÑ±¤¸¬ÆÍÆÎÆĞÆÔÆÖÆØÆÙÙéàÛäßå§è±ë«ïäïèõë"}; 
const static unsigned char PY_mb_ex_q[]=   {"ÆğÇ°ÆäÇ®ÇÇÆëÆîÇØÇüÇñÆİöÄÈ¥ÆøÈ«ÆÚÈºÇéÇøÇ¿ÇĞÈ¡ÆßÇóÇåÈ·È¨ÇÒÇà"}; 
const static unsigned char PY_mb_ex_qi[]=  {"ÆğÆäÆøÆÚÆßÆ÷ÆëÆæÆûÆóÆáÆÛÆìÆèÆôÆúÆç»ü¼©ÆÜÆİÆŞÆàÆâÆãÆåÆéÆêÆíÆîÆïÆñÆòÆõÆöÆùÆüÆıØ½ØÁÙ¹ÛßÜ»ÜÎÜùİ½İÂİİŞ­àÒá¨áªá«ãàä¿æëç²ç÷çùè½èçéÊêÈì¥ì÷í¬íÓñıòÓòàôëôìõè÷¢÷è"}; 
const static unsigned char PY_mb_ex_qia[]=  {"¿¨Ç¡ÆşÇ¢Ù¤İÖñÊ÷Ä"}; 
const static unsigned char PY_mb_ex_qian[]= {"Ç°Ç§Ç®Ç³Ç©Ç¨Ç¦Ç±Ç£Ç¯ÏËÇ´Ç¤Ç¥ÇªÇ«Ç¬Ç­Ç²ÇµÇ¶Ç·Ç¸Ù»ÙİÚäÜ·ÜÍÜçİ¡Şçá©ã¥ã»å¹åºå½ç×èıêùëÉí©îÔò¯óèóé"}; 
const static unsigned char PY_mb_ex_qiang[]= {"Ç¿½«Ç¹ÇÀÇ½Ç»ÇºÇ¼Ç¾ãŞæÍéÉê¨ìÁïºïÏïêñßòŞôÇõÄ"}; 
const static unsigned char PY_mb_ex_qiao[]=  {"ÇÅÇÆ¿ÇÇÉÇÃÇÇ½¶ÇÁÇÂÇÄÇÈÇÊÇËÇÌÇÍÇÎÇÏÈ¸ØäÚ½ÚÛÜñá½ã¸ã¾çØéÔíÍõÎ÷³"}; 
const static unsigned char PY_mb_ex_qie[]=  {"ÇĞÇÒÆöÇÑÇÓÇÔÙ¤Û§ã«ã»æªêüïÆóæ"}; 
const static unsigned char PY_mb_ex_qin[]=   {"Ç×ÇÖÇÚÇØÇÕÇÙÇÛÇÜÇİÇŞÇßÜËŞìßÄàºàßâÛäÚéÕï·ñæñûòûôÀ"}; 
const static unsigned char PY_mb_ex_qing[]= {"ÇéÇåÇàÇáÇãÇëÇ×ÇìÇâÇçÇäÇæÇèÇêÜÜàõéÑíàòßóÀóäôìö¥öë÷ô"}; 
const static unsigned char PY_mb_ex_qiong[]= {"ÇîÇíÚöÜäñ·òËóÌõ¼öÆ"}; 
const static unsigned char PY_mb_ex_qiu[]=  {"ÇóÇòÇïÇğ³ğ¹êÇñÇôÇõÇöÙ´ÛÏáìäĞåÏåÙé±êäò°òÇòøôÃôÜöú÷ü"}; 
const static unsigned char PY_mb_ex_qu[]=  {"È¥ÇøÈ¡ÇúÇÒÇşÇüÇ÷ÇıÈ¤ÇùÇûÈ¢È£ĞçÚ°Û¾ÜÄŞ¡Ş¾á«áéãÖè³êïë¬ëÔìîíáğ¶ñ³òĞó½ôğöÄ÷ñ"}; 
const static unsigned char PY_mb_ex_quan[]=  {"È«È¨È¦È°ÈªÈ©È§È¬È­È®È¯Ú¹Üõãªç¹éúî°îıòéóÜ÷Ü"}; 
const static unsigned char PY_mb_ex_que[]=  {"È·È´È±È²È³ÈµÈ¶È¸ã×ãÚí¨"}; 
const static unsigned char PY_mb_ex_qun[]=  {"ÈºÈ¹åÒ"}; 
const static unsigned char PY_mb_ex_r[]=   {"ÈËÈçÈÕÈîÈÙÈÏÈëÈÎÈÈÈ»ÈİÈÃÈóÈõÈ¼È¾ÈÀÈÆÈÊÈÜÈĞÈÔ"}; 
const static unsigned char PY_mb_ex_ran[]=  {"È»È¼È¾È½ÜÛòÅ÷×"}; 
const static unsigned char PY_mb_ex_rang[]=  {"ÈÃÈÀÈÂÈ¿ÈÁìüğ¦"}; 
const static unsigned char PY_mb_ex_rao[]=   {"ÈÆÈÅÈÄÜéæ¬èã"}; 
const static unsigned char PY_mb_ex_re[]=   {"ÈÈÈôÈÇ"}; 
const static unsigned char PY_mb_ex_ren[]=   {"ÈËÈÏÈÎÈÊÈĞÈÌÈÉÈÍÈÑÈÒØğÜóİØâ¿éíïşñÅ"}; 
const static unsigned char PY_mb_ex_reng[]=  {"ÈÔÈÓ"}; 
const static unsigned char PY_mb_ex_ri[]=   {"ÈÕ"}; 
const static unsigned char PY_mb_ex_rong[]=  {"ÈİÈÜÈÙÈÛÈÚÈŞÈÖÈ×ÈØÈßáÉáõéÅëÀòî"}; 
const static unsigned char PY_mb_ex_rou[]=  {"ÈâÈàÈáôÛõå÷·"}; 
const static unsigned char PY_mb_ex_ru[]=   {"ÈçÈëÈåÈéÈãÈäÈæÈèÈêÈìİêŞ¸àéä²äáå¦çÈï¨ñàò¬"}; 
const static unsigned char PY_mb_ex_ruan[]=  {"ÈíÈîëÃ"}; 
const static unsigned char PY_mb_ex_rui[]=   {"ÈğÈñÈïÜÇŞ¨èÄî£ò¸"}; 
const static unsigned char PY_mb_ex_run[]=  {"ÈóÈò"}; 
const static unsigned char PY_mb_ex_ruo[]=   {"ÈõÈôÙ¼óè"}; 
const static unsigned char PY_mb_ex_s[]=   {"ÊÇÉÏÊ±ËïËÎËÕÉòËÚÊ©ÉÛÙÜÉêÊ¦Ë§É³ËåÊ¯Ê·ÉúËµÉçÉîË®ËùÈıÊ®Ê¹ÊÂÊıËÄÊµÉÙÊ½É½ÊÖ"}; 
const static unsigned char PY_mb_ex_sa[]=    {"ÈöÈøÈ÷Ø¦ØíêıëÛìª"}; 
const static unsigned char PY_mb_ex_sai[]=  {"ÈûË¼ÈüÈùÈúàç"}; 
const static unsigned char PY_mb_ex_san[]=  {"ÈıÉ¢ÈşÉ¡âÌë§ôÖ"}; 
const static unsigned char PY_mb_ex_sang[]=  {"É£É¥É¤Şúíßòª"}; 
const static unsigned char PY_mb_ex_sao[]=   {"É¨ÉÒÉ¦É§É©Ü£çÒçØëığşöş"}; 
const static unsigned char PY_mb_ex_se[]=  {"É«ÈûÉªÉ¬ØÄï¤ğ£"}; 
const static unsigned char PY_mb_ex_sen[]=  {"É­"}; 
const static unsigned char PY_mb_ex_seng[]= {"É®"}; 
const static unsigned char PY_mb_ex_sh[]=   {"ÊıÏÃÉ½É¾ÉÌÉÙÕÙÉçÉèÉõÉùÊĞÊ¦Ê¼ÊÖÊÕÊØÊéÊôÊõÊäË«Ë®Ë­Ë¯Ë°Ë³"}; 
const static unsigned char PY_mb_ex_sha[]=  {"É³É±É°É¶É´É¯É²ÉµÉ·É¼ÏÃßşàÄêıì¦ï¡ğğôÄö®öè"}; 
const static unsigned char PY_mb_ex_shai[]=  {"É«É¸É¹"}; 
const static unsigned char PY_mb_ex_shan[]= {"É½µ¥ÉÁÉÆÉºÉÈÉÂµ§É»É¼É¾É¿ÉÀÉÃÉÄÉÅÉÇÉÉÕ¤ØßÚ¨Û·ÛïÜÏäúæ©æÓæóëşìøîÌğŞóµô®õÇ÷­"}; 
const static unsigned char PY_mb_ex_shang[]= {"ÉÏÉÌÉËÉĞÌÀÉÊÉÍÉÎÉÑÛğç´éäìØõü"}; 
const static unsigned char PY_mb_ex_shao[]= {"ÉÙÉÕÉÔÉÜÕÙÉÚÉÒÇÊÉÓÉÖÉ×ÉØÉÛÛ¿Üæäûè¼òÙóâô¹"}; 
const static unsigned char PY_mb_ex_she[]=  {"ÉçÉèÉäÉãÉàÕÛÉæÉáÉßÉİÉŞÉâÉåØÇÙÜß¡â¦äÜì¨î´÷ê"}; 
const static unsigned char PY_mb_ex_shen[]=  {"ÉîÉíÉñÊ²Éì²ÎÉõÉøÉòÉöÉóÉêÉ÷ÉéÉëÉïÉğÉôÚ·ÚÅİ·İØßÓäÉé©ëÏïòò×"}; 
const static unsigned char PY_mb_ex_sheng[]= {"ÉúÊ¤ÉùÊ¡ÉıÊ¢ÉşÊ£Ê¥Éü³ËÉûáÓäÅêÉíòóÏ"}; 
const static unsigned char PY_mb_ex_shi[]=   {"ÊÇÊ±Ê®Ê¹ÊÂÊµÊ½Ê¶ÊÀÊÔÊ¯Ê²Ê¾ÊĞÊ·Ê¦Ê¼Ê©Ê¿ÊÆÊªÊÊÊ³Ê§ÊÓÊÒÊÏËÆÊ´Ê«ÊÍÖ³Ê°ÊÎÊ»³×Ê¨Ê¬Ê­Ê¸ÊºÊÁÊÃÊÄÊÅÊÈÊÉÊËÊÌÊÑĞêÖÅÚÖÛõİªİéß±éøêÛìÂîæó§óÂóßõ§õ¹öåöõìêâ»"}; 
const static unsigned char PY_mb_ex_shou[]=  {"ÊÖÊÜÊÕÊ×ÊØÊìÊÚÊÙÊŞÊÛÊİá÷ç·ô¼"}; 
const static unsigned char PY_mb_ex_shu[]=  {"ÊıÊéÊ÷ÊôÊõÊäÊöÊìÊøÊóÊèÊâÊæÊßÊíÊåÊğÊàÊáÊãÊçÊêÊëÊîÊïÊñÊòÊùÊúÊûÊüÊşË¡ÓáØ­Ù¿ÛÓİÄŞóãğäøæ­ç£ë¨ëòì¯ïø"}; 
const static unsigned char PY_mb_ex_shua[]= {"Ë¢Ë£à§"}; 
const static unsigned char PY_mb_ex_shuai[]= {"Ë¥Ë§Ë¤Ë¦ó°"}; 
const static unsigned char PY_mb_ex_shuan[]= {"Ë¨Ë©ãÅäÌ"}; 
const static unsigned char PY_mb_ex_shuang[]={"Ë«ËªË¬ãñæ×"}; 
const static unsigned char PY_mb_ex_shui[]= {"Ë®ËµË­Ë¯Ë°"}; 
const static unsigned char PY_mb_ex_shun[]=  {"Ë³Ë±Ë²Ë´"}; 
const static unsigned char PY_mb_ex_shuo[]=  {"ËµÊıË¶Ë·Ë¸İôŞ÷åùéÃîå"}; 
const static unsigned char PY_mb_ex_si[]=  {"ËÄË¼ËÀË¹Ë¿ËÆË¾ËÇÊ³Ë½²ŞËºË»ËÁËÂËÃËÅËÈØËÙ¹ÙîÛÌßĞãáãôäùæ¦æáçÁìëïÈğ¸ñêòÏóÓ"}; 
const static unsigned char PY_mb_ex_song[]=  {"ËÉËÍËÎËÌËÊËËËÏËĞÚ¡İ¿áÂáÔâìã¤äÁñµ"}; 
const static unsigned char PY_mb_ex_sou[]=  {"ËÑËÒËÓËÔÛÅŞ´à²àÕâÈäÑì¬î¤ïËòô"}; 
const static unsigned char PY_mb_ex_su[]=  {"ËØËÙËÕËÜËõË×ËßËŞËàËÖËÚËÛËİÙíÚÕİøà¼ãºä³óùö¢öÕ"}; 
const static unsigned char PY_mb_ex_suan[]=  {"ËãËáËââ¡"}; 
const static unsigned char PY_mb_ex_sui[]=  {"ËæËëËéËäËêÄòËåËçËèËìËíËîÚÇİ´å¡åäìİíõî¡"}; 
const static unsigned char PY_mb_ex_sun[]=  {"ËğËïËñİ¥áøâ¸é¾öÀ"}; 
const static unsigned char PY_mb_ex_suo[]=  {"ËùËõËøË÷É¯ËòËóËôËößïàÂàÊæ¶èøêıíüôÈ"}; 
const static unsigned char PY_mb_ex_t[]=  {"ËûÍ¬ËüÌÆÌ·ÌÕÌÀÍ¯Í¿ÌïÙ¡ÌáÌâÌåÍ¼ÌìÍ·Í¨ÌõÌØÍÁÍÅÍ³Ì¨ËıÌ«Ì×Ìú"}; 
const static unsigned char PY_mb_ex_ta[]=  {"ËûËüËıËşÌ¤ËúÌ¡Ì¢Ì£ÍØàªãËäâäğåİé½í³îèõÁ÷£"}; 
const static unsigned char PY_mb_ex_tai[]= {"Ì¨Ì«Ì¬Ì¥Ì§Ì©Ì¦ÌªÌ­Û¢Ş·ß¾ææëÄìÆîÑõÌöØ"}; 
const static unsigned char PY_mb_ex_tan[]= {"Ì¸Ì¼Ì½Ì¿µ¯Ì¹Ì°Ì²Ì®Ì¯Ì±Ì³Ì´ÌµÌ¶Ì·ÌºÌ»Ì¾Û°å£ê¼ìşîãïÄïâ"}; 
const static unsigned char PY_mb_ex_tang[]= {"ÌÃÌÇÌÆÌÁÌÀÌÂÌÄÌÅÌÈÌÉÌÊÌËÌÌÙÎàûäçè©éÌï¦ïÛñíó¥ó«ôÊõ±"}; 
const static unsigned char PY_mb_ex_tao[]= {"Ì×ÌÖÌÓÌÕÌÑÌÒÌÍÌÎÌÏÌĞÌÔØ»ß¶ßûä¬èºìâ÷Ò"}; 
const static unsigned char PY_mb_ex_te[]=  {"ÌØß¯ìıí«ï«"}; 
const static unsigned char PY_mb_ex_teng[]= {"ÌÚÌÛÌÙÌÜëø"}; 
const static unsigned char PY_mb_ex_ti[]= {"ÌáÌâÌåÌæÌİÌèÌŞÌßÌàÌãÌäÌçÌéÌêÌëÙÃÜèã©åÑç°ç¾ğÃñÓõ®ŞĞ"}; 
const static unsigned char PY_mb_ex_tian[]=  {"ÌìÌïÌíÌîÌğµèÌñÌòÌóŞİãÃãÙéåî±"}; 
const static unsigned char PY_mb_ex_tiao[]= {"Ìõµ÷ÌøÌôÌöÌ÷Ù¬Üæìöñ»òèóÔôĞö¶öæ÷Ø"}; 
const static unsigned char PY_mb_ex_tie[]= {"ÌúÌùÌûİÆ÷Ñ"}; 
const static unsigned char PY_mb_ex_ting[]= {"ÌıÍ£Í¥Í¦Í¢ÌüÌşÍ¡Í¤Í§ÜğİãæÃèèî®îúòÑöª"}; 
const static unsigned char PY_mb_ex_tong[]= {"Í¬Í¨Í³Í­Í´Í²Í¯Í°¶±¶²Í©ÍªÍ«Í®Í±Ù¡ÙÚÛíÜíàÌá¼âúäüíÅ"}; 
const static unsigned char PY_mb_ex_tou[]=  {"Í·Í¶Í¸Íµî×÷»"}; 
const static unsigned char PY_mb_ex_tu[]=  {"Í¼ÍÁÍ»Í¾Í½Í¹Í¿ÍÂÍÃÍÀÍºÜ¢İ±İËîÊõ©"}; 
const static unsigned char PY_mb_ex_tuan[]=  {"ÍÅÍÄŞÒåèî¶Í¼°¸"}; 
const static unsigned char PY_mb_ex_tui[]=  {"ÍÆÍËÍÈÍÇÍÉÍÊß¯ìÕ"}; 
const static unsigned char PY_mb_ex_tun[]=  {"¶ÚÍÊÍÌÍÍÍÎÙÛâ½êÕëà"}; 
const static unsigned char PY_mb_ex_tuo[]=   {"ÍÑÍÏÍĞÍ×ÍÖ¶æÆÇÍÒÍÓÍÔÍÕÍØÍÙØ±Ù¢ÛçâÕãûèØèŞéÒíÈîèóêõ¢õÉö¾"}; 
const static unsigned char PY_mb_ex_w[]=  {"ÎªÎÒÎŞÍõÍôÎÀÎºÎâÎ¤ÎÅÎÚÚùÎÌÎÂÎéÎÊÍâÎåÎ»ÎïÎ¯ÍêÎÂÍòÎÄÍùÎäÎñÎ§ÎóÍí"}; 
const static unsigned char PY_mb_ex_wa[]=  {"ÍßÍÚ°¼ÍÛÍÜÍİÍŞÍàØôæ´ëğ"}; 
const static unsigned char PY_mb_ex_wai[]=  {"ÍâÍááË"}; 
const static unsigned char PY_mb_ex_wan[]=  {"ÍêÍòÍíÍäÍëÍçÍåÍìÍæÂûÃäÍãÍèÍéÍîÍïÍğÍñÍóØàÜ¹İ¸İÒæıçºçşëäîµòê"}; 
const static unsigned char PY_mb_ex_wang[]=  {"ÍùÍõÍûÍøÍüÍıÍöÍúÃ¢ÍôÍ÷ØèŞÌã¯éş÷Í"}; 
const static unsigned char PY_mb_ex_wei[]=  {"ÎªÎ»Î¯Î§Î¬Î¨ÎÀÎ¢Î°Î´ÍşÎ£Î²Î½Î¹Î¶Î¸ÎºÎ±ÒÅÎ¥Î¤Î·Î³Î¡Î¦Î©Î«Î­Î®ÎµÎ¼Î¾Î¿ÙËÚÃÚñÚóÛ×İÚŞ±àíàøá¡áËáÍâ«â¬ãÇãíä¢ä¶åÔæ¸çâè¸ê¦ì¿ìĞğôôºöÛ"}; 
const static unsigned char PY_mb_ex_wen[]=  {"ÎÊÎÂÎÄÎÈÎÆÎÅÎÃÎÁÎÇÎÉØØãÓãëè·ö©Ùï"}; 
const static unsigned char PY_mb_ex_weng[]=  {"ÎËÎÌÎÍİîŞ³"}; 
const static unsigned char PY_mb_ex_wo[]=  {"ÎÒÎÕÎÑÎÏÎĞÎÖÎÎÎÔÎÓÙÁİ«à¸á¢ä×ë¿íÒö»"}; 
const static unsigned char PY_mb_ex_wu[]=  {"ÎŞÎåÎïÎäÎñÎóÎéÎèÎÛÎòÎíÎç¶ñÎİÎÚÎâÍöÎÜÎÙÎ×ÎØÎßÎàÎáÎãÎæÎêÎëÎìÎîÎğØ£ØõÚãÚùÛØÜÌßíâĞâäâèä´å»åÃåüæÄæğè»êõì¶ìÉğÄğÍğíòÚöÈ÷ù"}; 
const static unsigned char PY_mb_ex_x[]=  {"Ñ§ÏÂĞĞĞíĞìĞÏÏôĞ¤Ñ¦ĞÜ½âÏîÏ°ÙşĞ¡ÏßĞÔĞÂĞ©ÏëĞÄÏòÏàÏµÏ¯ÏÖÎ÷ĞŞÏóÏÈĞÎ"}; 
const static unsigned char PY_mb_ex_xi[]=  {"ÏµÏ¯Î÷Ï°Ï¸ÎüÎöÏ²Ï´Ï³Ï¡Ï·Ï¶Ï£Ï¢Ï®ÎıÏ©ÎşÏ¤Ï§ÏªÀ°ÆÜÎôÎõÎøÎùÎúÎûÏ¥Ï¦Ï¨Ï«Ï¬Ï­Ï±ÙÒÙâÚôÛ­Üçİ¾İßİûŞÉßñáãâ¾ãÒä»äÀåïæÒçôéØêØêêì¤ì¨ìäìùìûğªñ¶ñÓòáó£ó¬ôªô¸ôËôÑôâõµõè÷û"}; 
const static unsigned char PY_mb_ex_xia[]=  {"ÏÂÏÄÏÅÏÁÏ¼»£Ï¹ÏºÏ»Ï½Ï¾Ï¿ÏÀÏÃßÈáòåÚè¦èÔíÌğıóÁ÷ï"}; 
const static unsigned char PY_mb_ex_xian[]= {"ÏßÏÖÏÈÏØÏŞ¼ûÏÔÏÊÏ×ÏÕÏİÏÜÏËÏ´ÏÆÏÒÏ³ÏÙÏÇÏÉÏÌÏÍÏÎÏÏÏĞÏÑÏÓÏÚÏÛÙşÜÈİ²Şºá­áıåßæµë¯ìŞììğÂğïò¹óÚôÌõ£õĞõÑö±"}; 
const static unsigned char PY_mb_ex_xiang[]= {"ÏëÏòÏàÏóÏìÏîÏäÏç½µÏãÏñÏêÏğÏíÏæÏáÏâÏåÏèÏéÏïÜ¼İÙâÃâÔæøç½ó­öß÷Ï"}; 
const static unsigned char PY_mb_ex_xiao[]= {"Ğ¡ÏûÏ÷Ğ§Ğ¦Ğ£ÏúÏõÏôĞ¤Ğ¢ÏöÏøÏùÏüÏıÏşĞ¥ßØáÅäìåĞæçç¯èÉèÕòÙóãóï÷Ì"}; 
const static unsigned char PY_mb_ex_xie[]=  {"Ğ©½âĞ´Ğ±Ğ»Ğ­ĞµÒ¶ÑªĞ¶Ğ¼Ğ¬ĞªĞ°Ğ²Ğ·Ğ¹ĞºÆõĞ¨Ğ«Ğ®Ğ¯Ğ³Ğ¸ÙÉÙôÛÄÛÆŞ¯ß¢â³âİäÍå¬åâç¥çÓé¿éÇò¡õó"}; 
const static unsigned char PY_mb_ex_xin[]=  {"ĞÂĞÄĞÅĞ¿Ğ¾ĞÁÑ°ĞÀĞ½ĞÃĞÆØ¶Ü°ê¿ì§ïâöÎ"}; 
const static unsigned char PY_mb_ex_xing[]= {"ĞĞĞÔĞÎĞÍĞÇĞËÊ¡ĞÑĞÕĞÒĞÈĞÉĞÊĞÌĞÏĞÓÚêÜôÜşß©â¼ã¬íÊ"}; 
const static unsigned char PY_mb_ex_xiong[]= {"ĞÛĞØĞÖĞ×ĞÜĞÙĞÚÜº"}; 
const static unsigned char PY_mb_ex_xiu[]=  {"ĞŞĞâĞİĞäĞãĞàËŞ³ôĞßĞáĞåßİá¶âÊâÓäåğ¼õ÷÷Û"}; 
const static unsigned char PY_mb_ex_xu[]=   {"ĞøĞíĞëĞèĞòĞéĞõĞóĞğĞîĞ÷ĞìĞæĞçĞêĞïĞñĞôĞöÓõÚ¼ÛÃÛ×Ş£äªä°äÓçïèòìãí¹íìñãôÚõ¯"}; 
const static unsigned char PY_mb_ex_xuan[]=  {"Ñ¡ĞıĞûĞüĞşÈ¯ĞùĞúÑ¢Ñ£Ñ¤ÙØÚÎİæŞïãùäÖäöè¯é¸êÑìÅìÓíÛîçïàğç"}; 
const static unsigned char PY_mb_ex_xue[]=  {"Ñ§ÑªÏ÷Ñ©Ñ¨Ñ¥Ñ¦ÚÊàåí´õ½÷¨"}; 
const static unsigned char PY_mb_ex_xun[]=  {"ÑµÑ®Ñ¸Ñ¶Ñ°Ñ­Ñ²»ç¿£Ñ«Ñ¬Ñ¯Ñ±Ñ³Ñ´Ñ·ÙãÛ¨Û÷Ü÷Ş¦Ş¹á¾áßâ´âşä­ä±êÖñ¿õ¸öàåæ"}; 
const static unsigned char PY_mb_ex_y[]=    {"Ò»ÓĞÑîÑÏÓàÑÖÒ¦ÑÕÔ¬ÑàÓáÔÀã¢ÀÖÒ¶ÑòÒüÓÈÓÃÒªÒÔÓÚÒ²ÑùÒåÓëÓÉÑ¹Ô±ÔÂÒâÓÖÔËÔ­ÓÍÑé"}; 
const static unsigned char PY_mb_ex_ya[]=   {"Ñ¹ÑÇÑ½ÑÀÑ¿ÑÅÑÁÑ¼Æ¥ÑºÑ»Ñ¾ÑÂÑÃÑÄÑÆÑÈØóÛëŞëá¬åÂæ«çğèâë²í¼íığé"}; 
const static unsigned char PY_mb_ex_yan[]=   {"ÑéÑĞÑÏÑÕÑÛÑÔÑÎÑİÑÒÑØÑÌÑÓÑÚÑçÑ×ÑàÑÜÇ¦ÑÉÑÊÑËÑÍÑÑÑÖÑÙÑŞÑßÑáÑâÑãÑäÑåÑæÑèÒóØÉØÍØßÙ²ÙÈÙğÚİÚçÛ±Û³Ü¾İÎáÃâûãÆãÕäÎäÙåûæÌçüéÜêÌëÙëçìÍî»óÛõ¦÷Ê÷Ğ÷ú"}; 
const static unsigned char PY_mb_ex_yang[]=  {"ÑùÑøÑõÑïÑóÑôÑòÑíÑëÑîÑöÑêÑìÑğÑñÑ÷Ñúáàâóãóì¾ìÈí¦òÕ÷±"}; 
const static unsigned char PY_mb_ex_yao[]=  {"ÒªÒ©Ô¼Ò¡ÑüÒ§ÑûÒ«ÌÕ½ÄÅ±ÑıÑşÒ¢Ò£Ò¤Ò¥Ò¦Ò¨Ô¿Ø²Ø³ßºáÊáæçÛçòèÃé÷ê×ëÈï¢ğÎñºôí÷¥"}; 
const static unsigned char PY_mb_ex_ye[]=   {"Ò²ÒµÒ³Ò¶ÒºÒ¹Ò°Ò¯Ò±Ğ°»©ÑÊÒ¬Ò­Ò®Ò´Ò·Ò¸×§ØÌÚËÚşŞŞêÊìÇîô"}; 
const static unsigned char PY_mb_ex_yi[]=   {"Ò»ÒÔÒåÒâÒÑÒÆÒ½ÒéÒÀÒ×ÒÒÒÕÒæÒìÒËÒÇÒÚÒÅÒÁÒÛÒÂÒÉÒàÒêÒíÒëÒÖÒäÒßÒ¼Ò¾Ò¿ÒÃÒÄÒÈÒÊÒÌÒÍÒÎÒÏÒĞÒÓÒØÒÙÒÜÒİÒŞÒáÒãÒçÒèÒîÒïØ×ØæØîØıÙ«Ú±ÛİÛüÜ²ÜÓÜèŞ²ŞÄŞÈŞÚß®ß½ß×ßŞàÉàæá»áÚâ¢âÂâøâùã¨äôåÆæäçËéìéóêİì¥ì½ìÚíôîÆï×ïîğêğùñ¯ñ´òæô¯ôàôèôı÷ğñÂ"}; 
const static unsigned char PY_mb_ex_yin[]=  {"ÒòÒıÒõÓ¡ÒôÒøÑÌÒşÒûÒñÒğÒóÒöÒ÷ÒùÒúÒüØ·Û´ÛßÛóÜ§ÜáßÅà³áşâ¹ä¦äÎë³î÷ñ«ñ¿ò¾ö¯ö¸"}; 
const static unsigned char PY_mb_ex_ying[]= {"Ó¦Ó°Ó²ÓªÓ¢Ó³Ó­Ó£Ó¤Ó¥Ó§Ó¨Ó©Ó«Ó¬Ó®Ó¯Ó±ÙøÛ«ÜãÜşİºİÓİöŞüàÓâßäŞäëå­çøè¬éºëôğĞñ¨ò£ó¿"}; 
const static unsigned char PY_mb_ex_yo[]=  {"ÓıÓ´à¡"}; 
const static unsigned char PY_mb_ex_yong[]=  {"ÓÃÓÂÓÀÓµÓ¿Ó¼Ó¹Ó¶Ó·Ó¸ÓºÓ»Ó½Ó¾ÓÁÙ¸ÛÕÜ­à¯ã¼çßïŞğ®÷«÷Ó"}; 
const static unsigned char PY_mb_ex_you[]=  {"ÓĞÓÉÓÖÓÍÓÒÓÑÓÅÓ×ÓÎÓÈÓÕÓÌÓÄÓÆÓÇÓÊÓËÓÏÓÓÓÔØÕØüÙ§İ¬İ¯İµŞÌßÏàóå¶èÖéàë»îğğàòÄòÊòöòøôíöÏ÷î÷ø"}; 
const static unsigned char PY_mb_ex_yu[]=  {"ÓÚÓëÓıÓãÓêÓñÓàÓöÔ¤ÓòÓïÓúÓæÓèÓğÓŞÓù¹ÈÓûÓîÎµÎ¾ÓØÓÙÓÛÓÜÓİÓßÓáÓâÓäÓåÓçÓéÓìÓíÓóÓôÓõÓ÷ÓøÓüÓşÔ¡Ô¢Ô£Ô¥Ô¦ÖàØ®Ø¹ØñÙ¶ÚÄÚÍİÇİÒİ÷ŞíàôàöáÎáüâÀâÅâ×ãĞå÷åıæ¥æúè¤êÅêìëéëòì£ì¶ìÏìÙìÛí²îÚğÁğÖğõğöñ¾ñÁòâòõóÄô§ô¨ö§ö¹í±"}; 
const static unsigned char PY_mb_ex_yuan[]= {"Ô±Ô­Ô²Ô´ÔªÔ¶Ô¸ÔºÔµÔ®Ô°Ô¹Ô§Ô¨Ô©Ô«Ô¬Ô¯Ô³Ô·ÛùÜ«Ü¾Şòà÷ãäæÂè¥éÚë¼íóğ°ó¢óîö½"}; 
const static unsigned char PY_mb_ex_yue[]=  {"ÔÂËµÔ½Ô¼ÀÖÔ¾Ô»ÔÄÔ¿ÔÀÔÁÔÃÙßå®èİéĞë¾îá"}; 
const static unsigned char PY_mb_ex_yun[]=  {"ÔËÔ±ÔÆÔÈ¾ùÔÊÔĞÔÅÔÇÔÉÔÌÔÍÔÎÔÏÛ©Ü¿áñã¢ã³ç¡è¹éæêÀëµìÙ"}; 
const static unsigned char PY_mb_ex_z[]=   {"ÔÚÕâÖ÷ÖĞÕÅÕÂÕÔÔøÖ£ÖÜ×ŞÖìÖÖ×Å×£Õç×¯×ÚÕ²ê°×æ×óÕ¹ÕùÕ½×÷ÕıÖ®ÖÆÖØ×éÖÎÖ»×Ó×ÔÖÊÖ¸"}; 
const static unsigned char PY_mb_ex_za[]=   {"ÔÓÔúÔÛÔÑÔÒÕ¦ßÆ"}; 
const static unsigned char PY_mb_ex_zai[]=   {"ÔÚÔÙÔØÔÔÔÖ×ĞÔÕÔ×áÌçŞ"}; 
const static unsigned char PY_mb_ex_zan[]=   {"ÔŞÔÛÔİÔÜŞÙè¶êÃô¢ôØôõöÉ"}; 
const static unsigned char PY_mb_ex_zang[]=  {"²ØÔàÔáÔßŞÊæàê°"}; 
const static unsigned char PY_mb_ex_zao[]= {"ÔìÔçÔâÔïÔäÔãÔæÔíÔåÔèÔéÔêÔëÔîßğ"}; 
const static unsigned char PY_mb_ex_ze[]=  {"ÔòÔñÔğ²àÔóÕ¦ØÆØÓßõàıåÅê¾óĞóåô·"}; 
const static unsigned char PY_mb_ex_zei[]= {"Ôô"}; 
const static unsigned char PY_mb_ex_zen[]=  {"ÔõÚÚ"}; 
const static unsigned char PY_mb_ex_zeng[]=  {"ÔöÔø×ÛÔ÷ÔùçÕêµîÀï­"}; 
const static unsigned char PY_mb_ex_zh[]=  {"ÔúÕ¨ÕªÕ®Õ½Õ¹³¤ÕÅÕÂÖÓ×ÅÕÕÕÒÕâÕæÕùÕıÕşÖ®ÖÆÖÎÖ»ÖĞÖÜÖİÖ÷×¡×¥×ª×¨×«×¬×°×´×¼"}; 
const static unsigned char PY_mb_ex_zha[]=  {"²éÔúÕ¨Õ¢Õ¡ÔşÔüÀ¯ÔûÔıÕ£Õ¤Õ¥Õ¦Õ§Õ©×õŞêß¸ßåßîà©é«íÄğäòÆ÷ş"}; 
const static unsigned char PY_mb_ex_zhai[]=  {"Õ¯²àÔñÕªÕ­Õ«µÔ¼ÀÕ¬Õ®íÎñ©"}; 
const static unsigned char PY_mb_ex_zhan[]= {"Õ½Õ¹Õ¾Õ¼Õ³²üÕ°Õ±Õ²Õ´ÕµÕ¶Õ·Õ¸ÕºÕ»Õ¿ÕÀÚŞŞøì¹"}; 
const static unsigned char PY_mb_ex_zhang[]={"³¤ÕÅÕÂÕÆÕÌÕÏÕÍÕÇÕËÕÁÕÃÕÄÕÈÕÉÕÊÕÎØëÛµá¤áÖâ¯æÑè°ó¯"}; 
const static unsigned char PY_mb_ex_zhao[]=  {"×ÅÕÕÕÒÕĞ³¯ÕÙÕÔ×¦ÕÖÕÓÕ×³°ÕÑÕØÚ¯ßúèşîÈóÉ"}; 
const static unsigned char PY_mb_ex_zhe[]=  {"Õâ×ÅÕßÕÛÕÜÕãÕÚÕİÕŞÕàÕáÚØß¡èÏéüíİğÑñŞòØô÷"}; 
const static unsigned char PY_mb_ex_zhen[]=  {"ÕæÕëÕóÕòÕñÕğÕäÕïÕåÕçÕèÕéÕêÕìÕíÕîÛÚİèä¥äÚçÇèåé©é»éôêâëÓëŞìõî³ğ¡ğ²óğ"}; 
const static unsigned char PY_mb_ex_zheng[]= {"ÕùÕıÕşÕûÖ¤Õ÷ÕôÖ¢¶¡Ö£ÕõÕöÕøÕúÕüÖ¡Úºá¿áçîÛï£óİ"}; 
const static unsigned char PY_mb_ex_zhi[]=   {"Ö®ÖÆÖÎÖ»ÖÊÖ¸Ö±Ö§Ö¯Ö¹ÖÁÖÃÖ¾ÖµÖªÊ¶Ö´Ö°Ö²Ö½ÖÂÖ¦Ö³Ö¬ÖÇÊÏÖ«ÖÈÖ·ÖÍÖ­Ö¥Ö¨Ö©Ö¶ÖºÖ¼Ö¿ÖÀÖÄÖÅÖÉÖËÖÌÖÏØ´ÚìÛ¤ÛúÜÆŞıàùáçâååéåëæïèÎè×èÙèäéòéùêŞëÕëùìíìóíéïôğºğëòÎôêõ¥õÅõÙõÜõôö£"}; 
const static unsigned char PY_mb_ex_zhong[]= {"ÖĞÖÖÖØÖÚÖÓÖÕÖÒÖ×ÖÙÖÑÖÔÚ£âìïñó®ô±õà"}; 
const static unsigned char PY_mb_ex_zhou[]=  {"ÖáÖÜÖŞÖİÖåÖèÖÛÖßÖàÖâÖãÖäÖæÖçİ§ßúæ¨æûç§ëĞíØô¦ôíôü"}; 
const static unsigned char PY_mb_ex_zhu[]=  {"Ö÷×¢Öø×¡ÖúÖíÖìÖıÊôÖêÖşÖùÊõ×¤Öğ×£ÖñÖüÖéÖîÖëÖïÖòÖóÖôÖõÖöÖûØùÙªÛ¥Üïä¨ä¾äóèÌéÆéÍìÄîùğæğññÒóÃóçô¶ôãõî÷æ"}; 
const static unsigned char PY_mb_ex_zhua[]=  {"×¥×¦ÎÎ"}; 
const static unsigned char PY_mb_ex_zhuai[]= {"×ªÒ·×§àÜ"}; 
const static unsigned char PY_mb_ex_zhuan[]= {"×ª×¨´«×©×«×¬×­ßùâÍãçò§"}; 
const static unsigned char PY_mb_ex_zhuang[]={"×°×´×³×¯×²´±×®×±Ù×ŞÊ"}; 
const static unsigned char PY_mb_ex_zhui[]=  {"×·×¶×µ×¸×¹×ºã·æíçÄ"}; 
const static unsigned char PY_mb_ex_zhun[]=  {"×¼ÍÍ×»ëÆñ¸"}; 
const static unsigned char PY_mb_ex_zhuo[]=  {"×ÅÖø×½×À½É×¾×¿×Á×Â×Ã×Ä×Æ×ÇÙ¾ÚÂßªä·äÃåªèşìÌìúí½ïí"}; 
const static unsigned char PY_mb_ex_zi[]=  {"×Ó×Ô×Ê×Ö×Ï×Ğ×Ñ×ËÖ¨×È×É×Ì×Í×Î×Ò×ÕÚÑÜëßÚáÑæ¢æÜç»è÷ê¢êßí§íöïÅïöñèóÊôÒôôõşö¤ö·öö÷Ú"}; 
const static unsigned char PY_mb_ex_zong[]=  {"×Ü×İ×Ú×Û×Ø×××ÙÙÌèÈëêôÕ"}; 
const static unsigned char PY_mb_ex_zou[]=  {"×ß×Ş×à×áÚÁÚîÛ¸æãöí"}; 
const static unsigned char PY_mb_ex_zu[]=  {"×é×å×ã×è×æ×â×ä×çÙŞİÏïß"}; 
const static unsigned char PY_mb_ex_zuan[]=  {"×ê×¬×ëß¬çÚõò"}; 
const static unsigned char PY_mb_ex_zui[]=  {"×î×ï×ì¶Ñ¾××íŞ©"}; 
const static unsigned char PY_mb_ex_zun[]=  {"×ğ×ñß¤é×÷®"}; 
const static unsigned char PY_mb_ex_zuo[]=  {"×÷×ö×ó×ù×ø×ò´éÔä´ê×Á×ô×õÚèßòàÜâôëÑìñóĞ"}; 

/*·ûºÅÊäÈë²éÑ¯Âë±í*/
#endif

#if 1
const T9EN_IDX t9EN_ex_index[] = {
		{"2","abcABC"},
		{"3","defDEF"},
		{"4","ghiGHI"},
		{"5","jklJKL"},
		{"6","mnoMNO"},
		{"7","pqrsPQRS"},
		{"8","tuvTUV"},
		{"9","wxyzWXYZ"}
};
#endif

#if 1
const T9PY_IDX t9PY_ex_index[] ={
				{"2","a",PY_mb_ex_a}, 
				{"2","b",PY_mb_ex_b},
				{"2","c",PY_mb_ex_c},
				{"3","d",PY_mb_ex_d}, 
				{"3","e",PY_mb_ex_e}, 
				{"3","f",PY_mb_ex_f}, 
				{"4","g",PY_mb_ex_g}, 
				{"4","h",PY_mb_ex_h},
				{"5","j",PY_mb_ex_j}, 
				{"5","k",PY_mb_ex_k}, 
				{"5","l",PY_mb_ex_l}, 
				{"6","m",PY_mb_ex_m}, 
				{"6","n",PY_mb_ex_n}, 
				{"6","o",PY_mb_ex_o}, 
				{"7","p",PY_mb_ex_p}, 
				{"7","q",PY_mb_ex_q}, 
				{"7","r",PY_mb_ex_r}, 
				{"7","s",PY_mb_ex_s}, 
				{"8","t",PY_mb_ex_t}, 
				{"9","w",PY_mb_ex_w},
				{"9","x",PY_mb_ex_x}, 
				{"9","y",PY_mb_ex_y}, 
				{"9","z",PY_mb_ex_z}, 
				{"22","ba",PY_mb_ex_ba},
				{"22","ca",PY_mb_ex_ca},
				{"23","ce",PY_mb_ex_ce},
				{"23","be",PY_mb_ex_bei},
				{"24","ai",PY_mb_ex_ai},
				{"24","bi",PY_mb_ex_bi},
				{"24","ch",PY_mb_ex_ch},
				{"24","ci",PY_mb_ex_ci}, 
				{"26","co",PY_mb_ex_cou},
				{"26","bo",PY_mb_ex_bo},
				{"26","an",PY_mb_ex_an},
				{"26","ao",PY_mb_ex_ao},
				{"28","bu",PY_mb_ex_bu},
				{"28","cu",PY_mb_ex_cu}, 
				{"32","da",PY_mb_ex_da}, 
				{"32","fa",PY_mb_ex_fa}, 
				{"33","de",PY_mb_ex_de}, 
				{"33","fe",PY_mb_ex_fei}, 
				{"34","di",PY_mb_ex_di}, 
				{"34","ei",PY_mb_ex_ei}, 
				{"36","en",PY_mb_ex_en}, 
				{"36","fo",PY_mb_ex_fo}, 
				{"37","er",PY_mb_ex_er}, 
				{"36","do",PY_mb_ex_dou}, 
				{"38","du",PY_mb_ex_du}, 
				{"38","fu",PY_mb_ex_fu}, 
				{"42","ga",PY_mb_ex_ga}, 
				{"42","ha",PY_mb_ex_ha}, 
				{"43","ge",PY_mb_ex_ge}, 
				{"43","he",PY_mb_ex_he}, 
				{"46","go",PY_mb_ex_gou}, 
				{"46","ho",PY_mb_ex_hou}, 
				{"48","gu",PY_mb_ex_gu}, 
				{"48","hu",PY_mb_ex_hu}, 
				{"52","ka",PY_mb_ex_ka}, 
				{"52","la",PY_mb_ex_la}, 
				{"53","le",PY_mb_ex_le}, 
				{"53","ke",PY_mb_ex_ke}, 
				{"54","ji",PY_mb_ex_ji}, 
				{"54","li",PY_mb_ex_li}, 
				{"56","ko",PY_mb_ex_kou}, 
				{"56","lo",PY_mb_ex_lo}, 
				{"58","lu",PY_mb_ex_lu}, 
				{"58","lv",PY_mb_ex_lv},
				{"58","ju",PY_mb_ex_ju}, 
				{"58","ku",PY_mb_ex_ku}, 
				{"62","ma",PY_mb_ex_ma}, 
				{"62","na",PY_mb_ex_na}, 
				{"63","me",PY_mb_ex_me}, 
				{"63","ne",PY_mb_ex_ne}, 
				{"64","mi",PY_mb_ex_mi}, 
				{"64","ni",PY_mb_ex_ni}, 
				{"66","mo",PY_mb_ex_mo},
				{"66","no",PY_mb_ex_nou}, 
				{"68","mu",PY_mb_ex_mu}, 
				{"68","nu",PY_mb_ex_nu}, 
				{"68","nv",PY_mb_ex_nv}, 
				{"68","ou",PY_mb_ex_ou}, 
				{"72","pa",PY_mb_ex_pa}, 
				{"72","pe",PY_mb_ex_pei}, 
				{"72","ra",PY_mb_ex_ran}, 
				{"72","sa",PY_mb_ex_sa}, 
				{"73","re",PY_mb_ex_re}, 
				{"73","se",PY_mb_ex_se}, 
				{"74","pi",PY_mb_ex_pi}, 
				{"74","qi",PY_mb_ex_qi}, 
				{"74","ri",PY_mb_ex_ri}, 
				{"74","sh",PY_mb_ex_sh}, 
				{"74","si",PY_mb_ex_si}, 
				{"76","so",PY_mb_ex_sou},
				{"76","po",PY_mb_ex_po}, 
				{"76","ro",PY_mb_ex_rou}, 
				{"78","pu",PY_mb_ex_pu}, 
				{"78","qu",PY_mb_ex_qu}, 
				{"78","ru",PY_mb_ex_ru}, 
				{"78","su",PY_mb_ex_su}, 
				{"82","ta",PY_mb_ex_ta}, 
				{"83","te",PY_mb_ex_te}, 
				{"84","ti",PY_mb_ex_ti}, 
				{"86","to",PY_mb_ex_tou}, 
				{"88","tu",PY_mb_ex_tu}, 
				{"92","wa",PY_mb_ex_wa}, 
				{"92","ya",PY_mb_ex_ya}, 
				{"92","za",PY_mb_ex_za}, 
				{"93","ze",PY_mb_ex_ze}, 
				{"93","ye",PY_mb_ex_ye}, 
				{"94","yi",PY_mb_ex_yi}, 
				{"94","xi",PY_mb_ex_xi}, 
				{"94","zh",PY_mb_ex_zh}, 
				{"94","zi",PY_mb_ex_zi}, 
				{"96","zo",PY_mb_ex_zou}, 
				{"96","wo",PY_mb_ex_wo}, 
				{"96","yo",PY_mb_ex_yo}, 
				{"98","wu",PY_mb_ex_wu}, 
				{"98","xu",PY_mb_ex_xu}, 
				{"98","yu",PY_mb_ex_yu}, 
				{"98","zu",PY_mb_ex_zu}, 
				{"224","bai",PY_mb_ex_bai},
				{"224","cai",PY_mb_ex_cai},
				{"226","can",PY_mb_ex_can},
				{"226","cao",PY_mb_ex_cao},
				{"226","ban",PY_mb_ex_ban},
				{"226","bao",PY_mb_ex_bao},
				{"234","bei",PY_mb_ex_bei},
				{"236","ben",PY_mb_ex_ben},
				{"236","cen",PY_mb_ex_cen},
				{"242","bia",PY_mb_ex_bian},
				{"242","cha",PY_mb_ex_cha},
				{"243","che",PY_mb_ex_che},
				{"243","bie",PY_mb_ex_bie},
				{"244","chi",PY_mb_ex_chi},
				{"246","bin",PY_mb_ex_bin},
				{"246","cho",PY_mb_ex_chou},
				{"248","chu",PY_mb_ex_chu},
				{"264","ang",PY_mb_ex_ang},
				{"266","con",PY_mb_ex_cong},
				{"268","cou",PY_mb_ex_cou}, 
				{"282","cua",PY_mb_ex_cuan}, 
				{"284","cui",PY_mb_ex_cui}, 
				{"286","cun",PY_mb_ex_cun}, 
				{"286","cuo",PY_mb_ex_cuo},
				{"324","dai",PY_mb_ex_dai}, 
				{"326","dan",PY_mb_ex_dan}, 
				{"326","dao",PY_mb_ex_dao}, 
				{"326","fan",PY_mb_ex_fan}, 
				{"334","fei",PY_mb_ex_fei}, 
				{"336","fen",PY_mb_ex_fen}, 
				{"336","den",PY_mb_ex_deng},
				{"342","dia",PY_mb_ex_dia}, 
				{"343","die",PY_mb_ex_die}, 
				{"346","din",PY_mb_ex_ding},
				{"348","diu",PY_mb_ex_diu}, 
				{"366","don",PY_mb_ex_dong}, 
				{"368","dou",PY_mb_ex_dou}, 
				{"368","fou",PY_mb_ex_fou}, 
				{"382","dua",PY_mb_ex_duan}, 
				{"384","dui",PY_mb_ex_dui}, 
				{"386","dun",PY_mb_ex_dun}, 
				{"386","duo",PY_mb_ex_duo}, 
				{"424","gai",PY_mb_ex_gai}, 
				{"424","hai",PY_mb_ex_hai}, 
				{"426","han",PY_mb_ex_han}, 
				{"426","gan",PY_mb_ex_gan}, 
				{"426","gao",PY_mb_ex_gao}, 
				{"426","hao",PY_mb_ex_hao}, 
				{"434","hei",PY_mb_ex_hei}, 
				{"434","gei",PY_mb_ex_gei}, 
				{"436","gen",PY_mb_ex_gen}, 
				{"436","hen",PY_mb_ex_hen}, 
				{"466","gon",PY_mb_ex_gong}, 
				{"466","hon",PY_mb_ex_hong}, 
				{"468","gou",PY_mb_ex_gou}, 
				{"468","hou",PY_mb_ex_hou}, 
				{"482","hua",PY_mb_ex_hua}, 
				{"482","gua",PY_mb_ex_gua}, 
				{"484","gui",PY_mb_ex_gui}, 
				{"484","hui",PY_mb_ex_hui}, 
				{"486","hun",PY_mb_ex_hun}, 
				{"486","huo",PY_mb_ex_huo}, 
				{"486","gun",PY_mb_ex_gun}, 
				{"486","guo",PY_mb_ex_guo}, 
				{"524","kai",PY_mb_ex_kai}, 
				{"524","lai",PY_mb_ex_lai}, 
				{"526","lan",PY_mb_ex_lan}, 
				{"526","kan",PY_mb_ex_kan}, 
				{"526","kao",PY_mb_ex_kao}, 
				{"526","lao",PY_mb_ex_lao}, 
				{"534","lei",PY_mb_ex_lei}, 
				{"536","len",PY_mb_ex_leng}, 
				{"536","ken",PY_mb_ex_ken}, 
				{"542","jia",PY_mb_ex_jia}, 
				{"542","lia",PY_mb_ex_lia}, 
				{"543","jie",PY_mb_ex_jie}, 
				{"543","lie",PY_mb_ex_lie}, 
				{"546","lin",PY_mb_ex_lin}, 
				{"546","jin",PY_mb_ex_jin}, 
				{"546","jio",PY_mb_ex_jiong}, 
				{"548","jiu",PY_mb_ex_jiu}, 
				{"548","liu",PY_mb_ex_liu}, 
				{"566","lon",PY_mb_ex_long}, 
				{"566","kon",PY_mb_ex_kong}, 
				{"568","kou",PY_mb_ex_kou}, 
				{"568","lou",PY_mb_ex_lou}, 
				{"582","lua",PY_mb_ex_luan}, 
				{"582","kua",PY_mb_ex_kua}, 
				{"582","jua",PY_mb_ex_juan}, 
				{"583","jue",PY_mb_ex_jue}, 
				{"583","lue",PY_mb_ex_lue}, 
				{"584","kui",PY_mb_ex_kui}, 
				{"586","lun",PY_mb_ex_lun}, 
				{"586","luo",PY_mb_ex_luo}, 
				{"586","kun",PY_mb_ex_kun}, 
				{"586","kuo",PY_mb_ex_kuo}, 
				{"586","jun",PY_mb_ex_jun}, 
				{"624","mai",PY_mb_ex_mai}, 
				{"624","nai",PY_mb_ex_nai}, 
				{"626","nan",PY_mb_ex_nan}, 
				{"626","man",PY_mb_ex_man}, 
				{"626","mao",PY_mb_ex_mao}, 
				{"626","nao",PY_mb_ex_nao}, 
				{"634","nei",PY_mb_ex_nei}, 
				{"634","mei",PY_mb_ex_mei}, 
				{"636","nen",PY_mb_ex_nen}, 
				{"636","men",PY_mb_ex_men}, 
				{"642","mia",PY_mb_ex_mian}, 
				{"642","nia",PY_mb_ex_nian}, 
				{"643","mie",PY_mb_ex_mie}, 
				{"643","nie",PY_mb_ex_nie}, 
				{"646","nin",PY_mb_ex_nin}, 
				{"646","min",PY_mb_ex_min}, 
				{"648","miu",PY_mb_ex_miu}, 
				{"648","niu",PY_mb_ex_niu}, 
				{"666","non",PY_mb_ex_nong}, 
				{"668","mou",PY_mb_ex_mou}, 
				{"668","nou",PY_mb_ex_nou}, 
				{"682","nua",PY_mb_ex_nuan}, 
				{"683","nve",PY_mb_ex_nve}, 
				{"686","nuo",PY_mb_ex_nuo}, 
				{"724","pai",PY_mb_ex_pai}, 
				{"734","pei",PY_mb_ex_pei}, 
				{"724","sai",PY_mb_ex_sai}, 
				{"726","san",PY_mb_ex_san}, 
				{"726","pan",PY_mb_ex_pan}, 
				{"726","pao",PY_mb_ex_pao}, 
				{"736","pen",PY_mb_ex_pen}, 
				{"726","ran",PY_mb_ex_ran}, 
				{"726","rao",PY_mb_ex_rao}, 
				{"726","sao",PY_mb_ex_sao},
				{"736","sen",PY_mb_ex_sen}, 
				{"736","ren",PY_mb_ex_ren}, 
				{"742","pia",PY_mb_ex_pian}, 
				{"742","qia",PY_mb_ex_qia}, 
				{"742","sha",PY_mb_ex_sha}, 
				{"743","pie",PY_mb_ex_pie}, 
				{"743","qie",PY_mb_ex_qie}, 
				{"743","she",PY_mb_ex_she}, 
				{"744","shi",PY_mb_ex_shi}, 
				{"748","shu",PY_mb_ex_shu}, 
				{"746","qin",PY_mb_ex_qin}, 
				{"746","pin",PY_mb_ex_pin}, 
				{"746","qio",PY_mb_ex_qiong}, 
				{"746","sho",PY_mb_ex_shou}, 
				{"748","qiu",PY_mb_ex_qiu}, 
				{"766","ron",PY_mb_ex_rong}, 
				{"768","pou",PY_mb_ex_pou}, 
				{"768","rou",PY_mb_ex_rou}, 
				{"766","son",PY_mb_ex_song},
				{"768","sou",PY_mb_ex_sou},
				{"782","sua",PY_mb_ex_suan}, 
				{"782","rua",PY_mb_ex_ruan}, 
				{"782","qua",PY_mb_ex_quan}, 
				{"783","que",PY_mb_ex_que}, 
				{"784","rui",PY_mb_ex_rui}, 
				{"784","sui",PY_mb_ex_sui}, 
				{"786","sun",PY_mb_ex_sun}, 
				{"786","suo",PY_mb_ex_suo}, 
				{"786","run",PY_mb_ex_run}, 
				{"786","ruo",PY_mb_ex_ruo}, 
				{"786","qun",PY_mb_ex_qun}, 
				{"824","tai",PY_mb_ex_tai}, 
				{"826","tan",PY_mb_ex_tan}, 
				{"826","tao",PY_mb_ex_tao}, 
				{"836","ten",PY_mb_ex_teng}, 
				{"842","tia",PY_mb_ex_tian}, 
				{"843","tie",PY_mb_ex_tie}, 
				{"846","tin",PY_mb_ex_ting}, 
				{"866","ton",PY_mb_ex_tong}, 
				{"868","tou",PY_mb_ex_tou}, 
				{"882","tua",PY_mb_ex_tuan}, 
				{"884","tui",PY_mb_ex_tui}, 
				{"886","tun",PY_mb_ex_tun}, 
				{"886","tuo",PY_mb_ex_tuo},
				{"924","wai",PY_mb_ex_wai}, 
				{"924","zai",PY_mb_ex_zai}, 
				{"926","zan",PY_mb_ex_zan}, 
				{"926","wan",PY_mb_ex_wan}, 
				{"926","yan",PY_mb_ex_yan}, 
				{"926","yao",PY_mb_ex_yao}, 
				{"926","zao",PY_mb_ex_zao}, 
				{"934","zei",PY_mb_ex_zei}, 
				{"934","wei",PY_mb_ex_wei}, 
				{"936","zen",PY_mb_ex_zen}, 
				{"936","wen",PY_mb_ex_wen}, 
				{"942","xia",PY_mb_ex_xia}, 
				{"942","zha",PY_mb_ex_zha}, 
				{"943","xie",PY_mb_ex_xie}, 
				{"943","zhe",PY_mb_ex_zhe}, 
				{"944","zhi",PY_mb_ex_zhi}, 
				{"946","zho",PY_mb_ex_zhou}, 
				{"946","xin",PY_mb_ex_xin}, 
				{"946","xio",PY_mb_ex_xiong}, 
				{"946","yin",PY_mb_ex_yin}, 
				{"948","xiu",PY_mb_ex_xiu}, 
				{"948","zhu",PY_mb_ex_zhu}, 
				{"966","yon",PY_mb_ex_yong}, 
				{"966","zon",PY_mb_ex_zong}, 
				{"968","you",PY_mb_ex_you}, 
				{"968","zou",PY_mb_ex_zou}, 
				{"982","zua",PY_mb_ex_zuan}, 
				{"982","yua",PY_mb_ex_yuan}, 
				{"982","xua",PY_mb_ex_xuan}, 
				{"983","xue",PY_mb_ex_xue}, 
				{"983","yue",PY_mb_ex_yue}, 
				{"984","zui",PY_mb_ex_zui}, 
				{"986","zun",PY_mb_ex_zun}, 
				{"986","zuo",PY_mb_ex_zuo}, 
				{"986","yun",PY_mb_ex_yun}, 
				{"986","xun",PY_mb_ex_xun}, 
				{"2264","bang",PY_mb_ex_bang},
				{"2364","beng",PY_mb_ex_beng},
				{"2426","bian",PY_mb_ex_bian},
				{"2426","biao",PY_mb_ex_biao},
				{"2464","bing",PY_mb_ex_bing},
				{"2264","cang",PY_mb_ex_cang},
				{"2364","ceng",PY_mb_ex_ceng},
				{"2424","chai",PY_mb_ex_chai},
				{"2426","chan",PY_mb_ex_chan},
				{"2426","chao",PY_mb_ex_chao},
				{"2436","chen",PY_mb_ex_chen},				
				{"2466","chon",PY_mb_ex_chong},
				{"2468","chou",PY_mb_ex_chou},
				{"2482","chua",PY_mb_ex_chuai},
				{"2484","chui",PY_mb_ex_chui},
				{"2486","chun",PY_mb_ex_chun},
				{"2486","chuo",PY_mb_ex_chuo},				
				{"2664","cong",PY_mb_ex_cong},
				{"2826","cuan",PY_mb_ex_cuan},
				{"3264","dang",PY_mb_ex_dang}, 
				{"3264","fang",PY_mb_ex_fang}, 
				{"3364","feng",PY_mb_ex_feng}, 
				{"3364","deng",PY_mb_ex_deng}, 
				{"3426","dian",PY_mb_ex_dian}, 
				{"3426","diao",PY_mb_ex_diao}, 
				{"3464","ding",PY_mb_ex_ding}, 
				{"3664","dong",PY_mb_ex_dong}, 
				{"3826","duan",PY_mb_ex_duan}, 
				{"4264","gang",PY_mb_ex_gang}, 
				{"4364","geng",PY_mb_ex_geng}, 
				{"4664","gong",PY_mb_ex_gong}, 
				{"4824","guai",PY_mb_ex_guai}, 
				{"4826","guan",PY_mb_ex_guan}, 
				{"4264","hang",PY_mb_ex_hang}, 
				{"4364","heng",PY_mb_ex_heng}, 
				{"4664","hong",PY_mb_ex_hong}, 
				{"4824","huai",PY_mb_ex_huai}, 
				{"4826","huan",PY_mb_ex_huan},
				{"5264","kang",PY_mb_ex_kang}, 
				{"5264","lang",PY_mb_ex_lang}, 
				{"5364","leng",PY_mb_ex_leng}, 
				{"5364","keng",PY_mb_ex_keng}, 
				{"5426","lian",PY_mb_ex_lian}, 
				{"5426","liao",PY_mb_ex_liao}, 
				{"5426","jian",PY_mb_ex_jian}, 
				{"5426","jiao",PY_mb_ex_jiao}, 
				{"5464","jing",PY_mb_ex_jing}, 
				{"5464","ling",PY_mb_ex_ling}, 
				{"5466","jion",PY_mb_ex_jiong}, 
				{"5664","long",PY_mb_ex_long}, 
				{"5664","kong",PY_mb_ex_kong}, 
				{"5824","kuai",PY_mb_ex_kuai}, 
				{"5826","kuan",PY_mb_ex_kuan}, 
				{"5826","luan",PY_mb_ex_luan}, 
				{"5826","juan",PY_mb_ex_juan}, 
				{"6264","mang",PY_mb_ex_mang}, 
				{"6264","nang",PY_mb_ex_nang}, 
				{"6364","meng",PY_mb_ex_meng}, 
				{"6364","neng",PY_mb_ex_neng}, 
				{"6426","mian",PY_mb_ex_mian}, 
				{"6426","miao",PY_mb_ex_miao}, 
				{"6426","nian",PY_mb_ex_nian}, 
				{"6426","niao",PY_mb_ex_niao}, 
				{"6464","ming",PY_mb_ex_ming}, 
				{"6464","ning",PY_mb_ex_ning}, 
				{"6664","nong",PY_mb_ex_nong}, 
				{"6826","nuan",PY_mb_ex_nuan}, 
				{"7264","pang",PY_mb_ex_pang}, 
				{"7364","peng",PY_mb_ex_peng}, 
				{"7264","rang",PY_mb_ex_rang}, 
				{"7264","sang",PY_mb_ex_sang}, 
				{"7364","seng",PY_mb_ex_seng}, 
				{"7364","reng",PY_mb_ex_reng}, 
				{"7424","shai",PY_mb_ex_shai}, 
				{"7426","shan",PY_mb_ex_shan}, 
				{"7426","shao",PY_mb_ex_shao}, 
				{"7426","pian",PY_mb_ex_pian}, 
				{"7426","piao",PY_mb_ex_piao}, 
				{"7426","qian",PY_mb_ex_qian}, 
				{"7426","qiao",PY_mb_ex_qiao}, 
				{"7436","shen",PY_mb_ex_shen}, 
				{"7464","ping",PY_mb_ex_ping}, 
				{"7464","qing",PY_mb_ex_qing}, 
				{"7466","qion",PY_mb_ex_qiong}, 
				{"7468","shou",PY_mb_ex_shou}, 
				{"7482","shua",PY_mb_ex_shua}, 
				{"7484","shui",PY_mb_ex_shui}, 
				{"7486","shun",PY_mb_ex_shun}, 
				{"7486","shuo",PY_mb_ex_shuo}, 
				{"7664","rong",PY_mb_ex_rong}, 
				{"7664","song",PY_mb_ex_song},
				{"7826","suan",PY_mb_ex_suan}, 
				{"7826","quan",PY_mb_ex_quan}, 
				{"7826","ruan",PY_mb_ex_ruan}, 
				{"8264","tang",PY_mb_ex_tang}, 
				{"8364","teng",PY_mb_ex_teng}, 
				{"8426","tian",PY_mb_ex_tian}, 
				{"8426","tiao",PY_mb_ex_tiao}, 
				{"8464","ting",PY_mb_ex_ting}, 
				{"8664","tong",PY_mb_ex_tong}, 
				{"8826","tuan",PY_mb_ex_tuan}, 
				{"9264","wang",PY_mb_ex_wang}, 
				{"9264","zang",PY_mb_ex_zang}, 
				{"9264","yang",PY_mb_ex_yang}, 
				{"9364","weng",PY_mb_ex_weng}, 
				{"9364","zeng",PY_mb_ex_zeng}, 
				{"9424","zhai",PY_mb_ex_zhai}, 
				{"9426","zhan",PY_mb_ex_zhan}, 
				{"9426","zhao",PY_mb_ex_zhao}, 
				{"9426","xian",PY_mb_ex_xian}, 
				{"9426","xiao",PY_mb_ex_xiao}, 
				{"9436","zhen",PY_mb_ex_zhen}, 
				{"9464","xing",PY_mb_ex_xing}, 
				{"9464","ying",PY_mb_ex_ying}, 
				{"9466","zhon",PY_mb_ex_zhong}, 
				{"9468","zhou",PY_mb_ex_zhou}, 
				{"9482","zhua",PY_mb_ex_zhua}, 
				{"9484","zhui",PY_mb_ex_zhui}, 
				{"9486","zhun",PY_mb_ex_zhun}, 
				{"9486","zhuo",PY_mb_ex_zhuo}, 
				{"9466","xion",PY_mb_ex_xiong}, 
				{"9664","yong",PY_mb_ex_yong}, 
				{"9664","zong",PY_mb_ex_zong}, 
				{"9826","yuan",PY_mb_ex_yuan}, 
				{"9826","xuan",PY_mb_ex_xuan}, 
				{"9826","zuan",PY_mb_ex_zuan}, 
				{"94264","xiang",PY_mb_ex_xiang}, 
				{"94264","zhang",PY_mb_ex_zhang}, 
				{"94364","zheng",PY_mb_ex_zheng}, 
				{"94664","xiong",PY_mb_ex_xiong}, 
				{"94664","zhong",PY_mb_ex_zhong}, 
				{"94824","zhuai",PY_mb_ex_zhuai}, 
				{"94826","zhuan",PY_mb_ex_zhuan},				
				{"24264","chang",PY_mb_ex_chang},				
				{"24364","cheng",PY_mb_ex_cheng},
				{"24664","chong",PY_mb_ex_chong},
				{"24824","chuai",PY_mb_ex_chuai},
				{"24826","chuan",PY_mb_ex_chuan},
				{"48264","guang",PY_mb_ex_guang}, 
				{"48264","huang",PY_mb_ex_huang}, 
				{"54264","jiang",PY_mb_ex_jiang}, 
				{"54264","liang",PY_mb_ex_liang}, 
				{"54664","jiong",PY_mb_ex_jiong}, 
				{"58264","kuang",PY_mb_ex_kuang}, 
				{"64264","niang",PY_mb_ex_niang}, 
				{"74264","qiang",PY_mb_ex_qiang}, 
				{"74264","shang",PY_mb_ex_shang}, 
				{"74364","sheng",PY_mb_ex_sheng}, 
				{"74664","qiong",PY_mb_ex_qiong}, 
				{"74824","shuai",PY_mb_ex_shuai}, 
				{"74826","shuan",PY_mb_ex_shuan}, 
				{"248264","chuang",PY_mb_ex_chuang},
				{"748264","shuang",PY_mb_ex_shuang}, 
				{"948264","zhuang",PY_mb_ex_zhuang}, 
				{"","",PY_mb_ex_space} 
};
#endif 


