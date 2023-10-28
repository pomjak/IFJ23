# IFJ23 LL-grammar

   |        Rule_start |  ->   | ruleset                                                                        |
   | ----------------: | :---: | :----------------------------------------------------------------------------- |
   |         *program* |  ->   | *statement* *program*                                                          |
   |         *program* |  ->   | **func** **ID** **(** *param-list* *func-ret-type* **{** *func-body* *program* |
   |         *program* |  ->   | **EOF**                                                                        |
   |       *statement* |  ->   | **var** *define*                                                               |
   |       *statement* |  ->   | **let** *define*                                                               |
   |       *statement* |  ->   | **while** ***EXP*** **{** *block-body*                                         |
   |       *statement* |  ->   | **if** *cond-clause* **{** *block-body* **else** **{** *block_body*            |
   |          *define* |  ->   | **ID** *var-def-cont*                                                          |
   |    *var-def-cont* |  ->   | **:** *type* *opt-assign*                                                      |
   |    *var-def-cont* |  ->   | **=** ***EXP***                                                                |
   |      *opt-assign* |  ->   | **=** ***EXP***                                                                |
   |      *opt-assign* |  ->   | **ε**                                                                          |
   | *expression-type* |  ->   | **=** ***EXP***                                                                |
   | *expression-type* |  ->   | **(** *argument-list*                                                          |
   |     *cond-clause* |  ->   | ***EXP***                                                                      |
   |     *cond-clause* |  ->   | **let** **ID**                                                                 |
   |   *argument-list* |  ->   | *argument* *argument-next*                                                     |
   |   *argument-list* |  ->   | **)**                                                                          |
   |   *argument-next* |  ->   | **,** *argument* *argument-next*                                               |
   |   *argument-next* |  ->   | **)**                                                                          |
   |        *argument* |  ->   | **ID** *opt-arg*                                                               |
   |        *argument* |  ->   | *literal*                                                                      |
   |      *param-list* |  ->   | *param* *param_next*                                                           |
   |      *param-list* |  ->   | **)**                                                                          |
   |      *param-next* |  ->   | **,** *param* *param_next*                                                     |
   |      *param-next* |  ->   | **)**                                                                          |
   |           *param* |  ->   | **_** **ID** **:** *type*                                                      |
   |           *param* |  ->   | **ID** **ID** **:** *type*                                                     |
   |      *block-body* |  ->   | *statement* *block-body*                                                       |
   |      *block-body* |  ->   | **}**                                                                          |
   |       *func-body* |  ->   | *func_stmt* *func-body*                                                        |
   |       *func-body* |  ->   | **}**                                                                          |
   |       *func-stmt* |  ->   | **var** *define*                                                               |
   |       *func-stmt* |  ->   | **let** *define*                                                               |
   |       *func-stmt* |  ->   | **ID** *expression-type*                                                       |
   |       *func-stmt* |  ->   | **while** ***EXP*** **{** *func-body*                                          |
   |       *func-stmt* |  ->   | **if** *cond-clause* **{** *func-body* **else** **{** *func-body*              |
   |       *func-stmt* |  ->   | **return** *opt-ret*                                                           |
   |   *func-ret-type* |  ->   | **->** *type*                                                                  |
   |   *func-ret-type* |  ->   | **ε**                                                                          |
   |         *opt-ret* |  ->   | ***EXP***                                                                      |
   |         *opt-ret* |  ->   | **ε**                                                                          |
   |        *opt-type* |  ->   | **:** *type*                                                                   |
   |        *opt-type* |  ->   | **ε**                                                                          |
   |            *type* |  ->   | **Int***nilable*                                                               |
   |            *type* |  ->   | **Double***nilable*                                                            |
   |            *type* |  ->   | **String***nilable*                                                            |
   |         *nilable* |  ->   | **?**                                                                          |
   |         *nilable* |  ->   | **ε**                                                                          |
   |         *opt-arg* |  ->   | **:** *term*                                                                   |
   |         *opt-arg* |  ->   | **ε**                                                                          |
   |            *term* |  ->   | **ID**                                                                         |
   |            *term* |  ->   | *literal*                                                                      |
   |         *literal* |  ->   | **INT_LITERAL**                                                                |
   |         *literal* |  ->   | **STRING_LITERAL**                                                             |
   |         *literal* |  ->   | **DOUBLE_LITERAL**                                                             |
