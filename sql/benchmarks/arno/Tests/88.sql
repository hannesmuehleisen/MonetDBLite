SELECT MODEL306.is_mutagen, count(distinct MODEL306.model_id ) FROM MODEL MODEL306, BOND T1008290441960  WHERE MODEL306.model_id=T1008290441960.model_id AND MODEL306.logp='0' group by MODEL306.is_mutagen;
