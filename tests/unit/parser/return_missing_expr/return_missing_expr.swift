/*
 * Každá funkce s návratovou hodnotou vrací hodnotu výrazu z příkazu return, 
 * avšak v případě chybějící návratové hodnoty kvůli neprovedení žádného příkazu return, 
 * nebo provedením příkazu return, ale s výrazem špatného typu 
 * (typ návratové hodnoty neodpovídá návratovému typu funkce),
 * dochází k chybě 4.
 * Chybí-li ve funkci vracející hodnotu výraz výraz u příkazu return, vede to na chybu 6.
 */

func a() -> Int{
    return
}
// Error 6