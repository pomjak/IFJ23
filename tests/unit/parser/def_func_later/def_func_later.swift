/* 
 *Každá funkce musí být definovaná7, jinak končí analýza chybou 3. 
 * Definice funkce nemusí lexikálně předcházet kódu pro použití této funkce, 
 * tzv. volání funkce. Uvažujte například vzájemné rekurzivní volání funkcí 
 * (tj. funkce foo volá funkci bar, která opět může volat funkci foo). 
 * Funkce může být definovaná lexikálně až za jejím použitím, tzv. voláním funkce 
 * (příkaz volání je definován níže).
 */

aaa(5.5)

bbb(5)

ccc("d", with :"sd")

func aaa(_ x : Double) { }
func bbb(p param: Int) -> Int 
{ 
    return 3
}
func ccc(_ x : String, with y : String) -> String? 
{ 
    return "hi" 
}

//ERROR 0