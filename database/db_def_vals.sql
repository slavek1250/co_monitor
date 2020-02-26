insert into co_def_type values ('m', 'measurements');
insert into co_def_type values ('p', 'parameters');
insert into co_def_type values ('s', 'safety parameters');


-- Definitions of measured data
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'ds1', 'Podajnik', '°C', '28FF6C9FC0160477');
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'ds2', 'Piec', '°C',  '28FF92CD331604C5');
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'ds3', 'Dodatkowy ', '°C', '28A587040A000057');
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'ds4', 'Na zewnątrz', '°C', '28FF93D7C0160442');
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'exhaust', 'Spaliny - termopara', '°C', null);
insert into co_def (def_type, name, description, unit, dev_serial) values ('m', 'distance', 'Poziom paliwa', 'cm', null);

-- Main driver configuration backup
insert into co_def (def_type, name, description, unit, dev_serial) values ('p', 'feeder_revo_num', 'Liczba obrotów podajnika', 'RPC', null);
insert into co_def (def_type, name, description, unit, dev_serial) values ('p', 'feeder_break', 'Przerwa podajnika', 's', null);
insert into co_def (def_type, name, description, unit, dev_serial) values ('p', 'set_temp', 'Temperatura ustawiona', '°C', null);
insert into co_def (def_type, name, description, unit, dev_serial) values ('p', 'hist', 'Histareza', '°C', null);

-- Definitions of safety 
insert into co_def (def_type, name, description, unit, dev_serial) values ('s', 'fmt', 'Maksymalna temperatura podajnika (Feeder Max Temp.)', '°C', null);
insert into co_def (def_type, name, description, unit, dev_serial) values ('s', 'fmd', 'Maksymalna odległość paliwa od góry podajnika (Feeder Max Dist.)', 'cm', null);

set @id = (select id from co_def where name ='ds1');
update co_def set id_co_def_pair = @id where name = 'fmt';
set @id = (select id from co_def where name ='ds2');
update co_def set id_co_def_pair = @id where name = 'set_temp';
set @id = (select id from co_def where name ='distance');
update co_def set id_co_def_pair = @id where name = 'fmd';

-- Definitions of operations being taken after an event has occurred
insert into co_event_operation (name, description) values ('none', 'Nie podejmuję żadnej akcji.');
insert into co_event_operation (name, description) values ('off', 'Wyłączam podajnik.');
insert into co_event_operation (name, description) values ('empty_feeder', 'Opróżniam zasobnik.');

-- Definitions of events
insert into co_event (name, description, operator, co_def_id_to_comp, co_def_id_comp_to, co_event_operation_id) values (
	'feeder_temp_too_high',
	'Zbyt wysoka temperatura podajnika!',
	'>',
	(select id from co_def where name = 'ds1'),
	(select id from co_def where name = 'fmt'),
	(select id from co_event_operation where name = 'empty_feeder')
);

insert into co_event (name, description, operator, co_def_id_to_comp, co_def_id_comp_to, co_event_operation_id) values (
	'feeder_empty',
	'Podajnik jest pusty!',
	'>',
	(select id from co_def where name = 'distance'),
	(select id from co_def where name = 'fmd'),
	(select id from co_event_operation where name = 'none')
);

INSERT INTO `co_param` set `id` = NULL;


-- Example parameters values
insert into co_param () values();

insert into co_param_elem (value, co_param_id, co_def_id) values ( 2, (select max(id) from co_param), (select id from co_def where name = 'feeder_revo_num'));
insert into co_param_elem (value, co_param_id, co_def_id) values ( 110, (select max(id) from co_param), (select id from co_def where name = 'feeder_break'));
insert into co_param_elem (value, co_param_id, co_def_id) values ( 53, (select max(id) from co_param), (select id from co_def where name = 'set_temp'));
insert into co_param_elem (value, co_param_id, co_def_id) values ( 1, (select max(id) from co_param), (select id from co_def where name = 'hist'));

insert into co_param_elem (value, co_param_id, co_def_id) values ( 80, (select max(id) from co_param), (select id from co_def where name = 'fmt'));
insert into co_param_elem (value, co_param_id, co_def_id) values ( 110, (select max(id) from co_param), (select id from co_def where name = 'fmd'));

