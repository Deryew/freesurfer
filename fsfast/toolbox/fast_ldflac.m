function flac = fast_ldflac(flacfile)
% flac = fast_ldflac(flacfile)
%
% $Id: fast_ldflac.m,v 1.4 2004/10/25 04:54:38 greve Exp $

flac = [];
if(nargin > 1)
  fprintf('flac = fast_ldflac(flacfile)\n');
  return;
end

flac.name = '';
flac.fsd = '';
flac.runlistfile = '';
flac.funcstem = '';
flac.runlistfile = '';
flac.TR = [];
flac.mask = '';
flac.inorm = [];
flac.whiten = 0;
flac.fixacf = 0;
%flac.ev = []; % Leave commented
if(nargin == 0) return; end

fp = fopen(flacfile,'r');
if(fp == -1)
  fpritf('ERROR: could not open %s\n',flacfile);
  flac = [];
  return;
end

% Get the first line, should look like:
% FSFAST-FLAC 1
tline = fgetl(fp);
flacid = sscanf(tline,'%s',1);
if(~strcmp(flacid,'FSFAST-FLAC'))
  fprintf('ERROR: format error (id string) in flac file %s\n',flacfile);
  fclose(fid);
  flac = [];
  return;
end
flacversion = sscanf(tline,'%*s %d',1);
if(flacversion ~= 1)
  fprintf('ERROR: flac file %s, version = %d\n',flacfile,flacversion);
  fclose(fid);
  flac = [];
  return;
end

nthev = 1;  % Keep track of the number of EVs
while(1)
  % scroll through any blank lines or comments
  while(1)
    tline = fgetl(fp);
    if(~isempty(tline) & tline(1) ~= '#') break; end
  end
  if(tline(1) == -1) break; end

  key = sscanf(tline,'%s',1);
  %fprintf('key = %s\n',key);
  
  switch(key)
   case 'flacname',    flac.name        = sscanf(tline,'%*s %s',1);
   case 'fsd',         flac.fsd         = sscanf(tline,'%*s %s',1);
   case 'TR',          flac.TR          = sscanf(tline,'%*s %f',1);
   case 'funcstem',    flac.funcstem    = sscanf(tline,'%*s %s',1);
   case 'mask',        flac.mask        = sscanf(tline,'%*s %s',1);
   case 'inorm',       flac.inorm       = sscanf(tline,'%*s %f',1);
   case 'runlistfile', flac.runlistfile = sscanf(tline,'%*s %s',1);
   case 'whiten',      flac.whiten      = sscanf(tline,'%*s %d',1);
   case 'acfbins',     flac.acfbins     = sscanf(tline,'%*s %d',1);
   case 'fixacf',      flac.fixacf      = sscanf(tline,'%*s %d',1);
   case 'EV', 
    ev = flac_ev_parse(tline);
    if(isempty(ev)) flac=[]; return; end
    flac.ev(nthev) = ev;
    nthev = nthev + 1;
   case 'CONTRAST-BEGIN', 
    flac = load_contrast(fp,flac);
    if(isempty(flac)) return; end
   otherwise
    fprintf('INFO: key %s unrecognized, skipping\n',key);
  end
end % while (1)

fclose(fp);

if(isempty(flac.fsd)) flac.fsd = 'bold'; end 
if(isempty(flac.funcstem)) 
  fprintf('ERROR: no funcstem specified in %s\n',flacfile);
  flac = [];
end

nevs = length(flac.ev);
if(nevs == 0)
  fprintf('ERROR: no EVs specified in %s\n',flacfile);
  flac = [];
end

ncon = length(flac.con);
if(ncon == 0)
  fprintf('WARNING: no contrasts in FLAC file %s\n',flacfile);
  return;
end

% Check each contrast
for nthcon = 1:ncon

  % Make sure that each EV in the contrast is an EV in the FLAC
  for nthev = 1:length(flac.con(nthcon).ev)
    evindex = flac_evindex(flac,flac.con(nthcon).ev(nthev).name);
    if(isempty(evindex))
      fprintf('ERROR: in FLAC file %s, Contrast %s\n',...
	      flacfile,flac.con(nthcon).name);
      fprintf('Contrast EV %s is not in the model\n',...
	      flac.con(nthcon).ev(nthev).name);
    end
  end
  
  % Compute the contrast matrices
  flactmp = flac_conmat(flac,nthcon);
  if(isempty(flactmp))
    fprintf('ERROR: with contrast %s in %s\n',...
	    flac.con(nthcon).name,flacfile);
    flac = [];
    return;
  end
  flac = flactmp;
end

% Should do some tests here to make sure names are not rep,
% contrasts, etc



return;

%------------------------------------------------%
function flac = load_contrast(fp,flac)
  if(~isfield(flac,'con')) ncon = 0; 
  else ncon = length(flac.con);
  end
  ncon = ncon + 1;

  flac.con(ncon).sumev    = 0;
  flac.con(ncon).sumevreg = 0;
  
  nthev = 1;
  while(1)
    % scroll through any blank lines or comments
    while(1)
      tline = fgetl(fp);
      if(~isempty(tline) & tline(1) ~= '#') break; end
    end
    if(tline(1) == -1) break; end

    key = sscanf(tline,'%s',1);
    switch(key)
     case 'NAME'
      [tmp c] = sscanfitem(tline,2);
      if(c ~= 1) fprintf('FLAC-CON format error\n');flac=[];return;end
      flac.con(ncon).name = tmp;     
     case 'SUMEV'
      [tmp c] = sscanfitem(tline,2);
      if(c ~= 1) fprintf('FLAC-CON format error\n');flac=[];return;end
      flac.con(ncon).sumev = sscanf(tmp,'%d',1);
     case 'SUMEVREG'
      [tmp c] = sscanfitem(tline,2);
      if(c ~= 1) fprintf('FLAC-CON format error\n');flac=[];return;end
      flac.con(ncon).sumevreg = sscanf(tmp,'%d',1);
     case 'EV'
      [tmp c] = sscanfitem(tline,2);
      if(c ~= 1) fprintf('FLAC-CON format error\n');flac=[];return;end
      flac.con(ncon).ev(nthev).name = tmp;
      [tmp c] = sscanfitem(tline,3);
      if(c ~= 1) fprintf('FLAC-CON format error\n');flac=[];return;end
      flac.con(ncon).ev(nthev).evw = sscanf(tmp,'%f',1);
      nthevrw = 0;
      while(1)
	[tmp c] = sscanfitem(tline,nthevrw+4);
	if(~c) break; end
	nthevrw = nthevrw + 1;
	flac.con(ncon).ev(nthev).evrw(nthevrw) = sscanf(tmp,'%f',1);
      end
      if(nthevrw == 0) flac.con(ncon).ev(nthev).evrw = []; end
      nthev = nthev+1;
     case 'EVRW' % Default EV Regressor Weights
      nthevrw = 0;
      while(1)
	[tmp c] = sscanfitem(tline,nthevrw+2);
	if(~c) break; end
	nthevrw = nthevrw + 1;
	flac.con(ncon).evrw(nthevrw) = sscanf(tmp,'%f',1);
      end
      if(nthevrw == 0)
	fprintf('ERROR: EVRW has no weights\n');
	flac = [];
	return;
      end
     case 'CONTRAST-END'
      if(~isfield(flac.con(ncon),'evrw')) flac.con(ncon).evrw = []; end
      return;
     otherwise
      fprintf('ERROR: key %s in contrast %d not recognized\n',key,ncon);
      flac = [];
      return;
    end
  end
  

return

% If one per-evrw is spec, then all must be spec (why?)
% If global evrw is spec, then per-evrw not allowed (why?)
% If global evrw is spec, then all non-zero EVs must have same
%  number of regressors equal to the number of weights
% Per-EVRW is not allowed for F-tests
% If Per-EVRW is not spec, then defaults to all ones.
% If Global-EVRW are not spec, then defaults to all ones.
% The number of weights in EVRW for an EV be same as nreg