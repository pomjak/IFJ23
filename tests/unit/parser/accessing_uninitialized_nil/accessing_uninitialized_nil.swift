/*
 *V případě chybějícího inicializačního výrazu 
 * se u proměnných typu zahrnujícího nil provádí 
 * implicitní inicializace hodnotou nil. 
 * Proměnné ostatních typů nejsou bez inicializačního 
 * výrazu inicializovány. Pokus o přístup k hodnotě 
 * neinicializované proměnné způsobí chybu 5.
*/
func a(_ arg :String?){}
var x : String//x should have value of nil 
a(x) //should be okey
//ERROR 0