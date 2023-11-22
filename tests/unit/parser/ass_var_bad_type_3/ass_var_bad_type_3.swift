/*
 * Příkaz přiřazení: id = výraz 
 * Sémantika příkazu je následující: 
 * Příkaz provádí vyhodnocení výrazu výraz (viz kapitola 5) a přiřazení jeho hodnoty do levého operandu id, 
 * který je modifikovatelnou proměnnou (tj. definovanou pomocí klíčového slova var). 
 * Pokud není hodnota výrazu typově kompatibilní s typem proměnné id, dojde k chybě 7.
*/
let x : Double = 1

// Error 0