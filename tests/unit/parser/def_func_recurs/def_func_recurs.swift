/* 
 *Každá funkce musí být definovaná7, jinak končí analýza chybou 3. 
 * Definice funkce nemusí lexikálně předcházet kódu pro použití této funkce, 
 * tzv. volání funkce. Uvažujte například vzájemné rekurzivní volání funkcí 
 * (tj. funkce foo volá funkci bar, která opět může volat funkci foo). 
 * Funkce může být definovaná lexikálně až za jejím použitím, tzv. voláním funkce 
 * (příkaz volání je definován níže).
 */

func bar(with param : String) -> String 
{ 
    let r : String = foo(param) 
    return r 
} 
func foo(_ par : String) -> String 
{ 
    let ret = bar(with: par) 
    return ret 
} 
bar(with: "ahoj")
//ERROR 0