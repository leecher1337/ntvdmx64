double_word sas_dw_at(IN sys_addr Address);

GLOBAL void sas_PRWS (PHY_ADDR Source, PHY_ADDR Destination, PHY_ADDR Length);

VOID 
sas_fills(
    IN sys_addr Destination,
    IN half_word Value,
    IN sys_addr Length
    );

VOID
sas_fillsw(
    IN sys_addr Destination,
    IN word Value,
    IN sys_addr Length
    );
    
VOID
sas_fillsdw(
    IN sys_addr Destination,
    IN IU32 Value,
    IN sys_addr Length
    );
    
GLOBAL IU8 sas_PR8 (PHY_ADDR addr);
GLOBAL IU16 sas_PR16 (PHY_ADDR addr);
GLOBAL void sas_PW8 (PHY_ADDR addr, IU8 val);
GLOBAL void sas_PWS (PHY_ADDR Destination, IU8 * Source, PHY_ADDR Length);
GLOBAL void sas_PRS (PHY_ADDR Source, IU8 * Destination, PHY_ADDR Length);
GLOBAL void sas_PW16 (PHY_ADDR addr, IU16 val);
GLOBAL IU32 sas_PR32 (PHY_ADDR addr);
GLOBAL void sas_PW32 (PHY_ADDR addr, IU32 val);
