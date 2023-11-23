/*
 * Příkaz definice proměnné: 
    *    let id : typ = výraz 
    *    var id : typ = výraz
 *
 * Sémantika příkazu je následující: Příkaz nově definuje globální/lokální proměnnou, 
 * která v daném bloku ještě nebyla definována (lze takto překrýt proměnnou definovanou v případném obklopujícím bloku),
 * jinak dojde k chybě 3. Příkaz provádí vyhodnocení případného výrazu (viz kapitola 5) a přiřazení jeho hodnoty id. 
 * Levý operand id je pro klíčové slovo var proměnná (tzv. l-hodnota) nebo pro klíčové slovo let nemodifikovatelná 
 * proměnná. Hodnotu nemodifikovatelné proměnné nelze po inicializaci již dále měnit. Je-li možné při překladu 
 * odvodit typ výrazu přiřazovaného do proměnné, tak je možné část ’: typ’ vypustit a typ proměnné id odvodit. 
 * Chybí-li výraz i typ, jedná se o syntaktickou chybu. Nelze-li chybějící typ odvodit (např. z hodnoty nil), 
 * dojde k chybě 8. Není-li typ zapsaného výrazu kompatibilní s uvedeným typem typ, nastane chyba 7. 
 * Pokud je typ proměnné zadán, lze část ’= výraz’ vynechat.
 */

var a // Error 2
