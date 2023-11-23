/*
 * Místo pravdivostního výrazu výraz lze alternativně použít syntaxi: 
 *      ’let id’, 
 * kde id zastupuje dříve definovanou (nemodifikovatelnou) proměnnou. 
 * Je-li pak proměnná id hodnoty nil, vykoná se sekvence_příkazů2, 
 * jinak se vykoná sekvence_příkazů1, kde navíc bude typ id upraven tak, že nebude (pouze v tomto bloku) zahrnovat
 * hodnotu nil (tj. např. proměnná původního typu String? bude v tomto bloku typu String). 
 * 
 */

let x : Int? = 6

if let x {}
else{}