# IFJ23 LL-grammar

   | Rule_start         | -> | ruleset                                                             |
   | ------------------:|:--:|:--------------------------------------------------------------------|
   | *program*          | -> | *statement-list*                                                    |
   | *statement-list*   | -> | *statement* *statement-list*                                        |
   | *statement-list*   | -> | *func-def* *statement-list*                                         |
   | *statement-list*   | -> | **EOF**                                                             |
   | *statement*        | -> | *var-def*                                                           |
   | *statement*        | -> | *id-construct*                                                      |
   | *statement*        | -> | *while-loop*                                                        |
   | *statement*        | -> | *conditional*                                                       |
   | *var-def*          | -> | *mutable* **ID** *var-def-cont*                                     |
   | *var-def-cont*     | -> | **:** *type* *opt-assign*                                           |
   | *var-def-cont*     | -> | *assign-exp*                                                        |
   | *opt-assign*       | -> | *assign-exp*                                                        |
   | *opt-assign*       | -> | **ε**                                                               |
   | *id-construct*     | -> | **ID** *expression-type*                                            |
   | *expression-type*  | -> | *assign-exp*                                                        |
   | *expression-type*  | -> | **(** *argument-list*                                               |
   | *while-loop*       | -> | **while** ***EXP*** **{** *block-body*                              |
   | *conditional*      | -> | **if** *cond-clause* **{** *block-body* **else** **{** *block_body* |
   | *cond-clause*      | -> | ***EXP***                                                           |
   | *cond-clause*      | -> | **let** **ID**                                                      |
   | *argument-list*    | -> | *argument* *argument-next*                                          |
   | *argument-next*    | -> | **,** *argument* *argument-next*                                    |
   | *argument-next*    | -> | **)**                                                               |
   | *argument*         | -> | **ID** *opt-arg*                                                    |
   | *argument*         | -> | *literal*                                                           |
   | *func-def*         | -> | **func** **ID**(*param-list* *func-ret-type* **{** *func-body*      |
   | *param-list*       | -> | *param* *param_next*                                                |
   | *param-next*       | -> | **,** *param* *param_next*                                          |
   | *param-next*       | -> | **)**                                                               |
   | *param*            | -> | *param_ext_id* **ID** **:** *type*                                  |
   | *param_ext_id*     | -> | **_**                                                               |
   | *param_ext_id*     | -> | **PARAM_ID**                                                        |
   | *block-body*       | -> | *statement* *block-body*                                            |
   | *block-body*       | -> | **}**                                                               |
   | *func-body*        | -> | *func_stmt* *func-body*                                             |
   | *func-body*        | -> | **}**                                                               |
   | *func-stmt*        | -> | *var-def*                                                           |
   | *func-stmt*        | -> | *id-construct*                                                      |
   | *func-stmt*        | -> | **while** ***EXP*** **{** *func-body*                               |
   | *func-stmt*        | -> | **if** *cond-clause* **{** *func-body* **else** **{** *func-body*   |
   | *func-stmt*        | -> | **return** *opt-ret*                                                |
   | *func-ret-type*    | -> | **->** *type*                                                       |
   | *func-ret-type*    | -> | **ε**                                                               |
   | *opt-ret*          | -> | ***EXP***                                                           |
   | *opt-ret*          | -> | **ε**                                                               |
   | *opt-type*         | -> | **:** *type*                                                        |
   | *opt-type*         | -> | **ε**                                                               |
   | *type*             | -> | **Int***nilable*                                                    |
   | *type*             | -> | **Double***nilable*                                                 |
   | *type*             | -> | **String***nilable*                                                 |
   | *nilable*          | -> | **?**                                                               |
   | *nilable*          | -> | **ε**                                                               |
   | *opt-arg*          | -> | **:** *term*                                                        |
   | *opt-arg*          | -> | **ε**                                                               |
   | *term*             | -> | **ID**                                                              |
   | *term*             | -> | *literal*                                                           |
   | *mutable*          | -> | **let**                                                             |
   | *mutable*          | -> | **var**                                                             |
   | *literal*          | -> | **INT_LITERAL**                                                     |
   | *literal*          | -> | **STRING_LITERAL**                                                  |
   | *literal*          | -> | **DOUBLE_LITERAL**                                                  |
