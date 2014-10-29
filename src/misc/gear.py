#####################################################33
# Base Status:
#  - pstats: Primary attributes
#  - vitaly: Vitality
#  - lifepc: Life %
#  - crithc: Critical Hit Chance
#  - crithd: Critial Hit Damage
#  - atcksp: Increased Attack Speed
#  - killxp: Kill Experience
#  - goldfd: Gold Find
#  - eledmg: Elemental Damage
#  - skldmg: Skill Damage
#  - allres: All Resistance
#  - armorr: Armor
#  - basdmg: Base Damage
#
# Metrics:
#  - DMG: Damage
#  - SDM: Skill Damage
#  - EDM: Elemental Damage
#  - MDM: Skill+Elemental Damage
#  - ATS: Hits per Seconds
#  - TGS: Toughness
#  - HTP: HitPoints
#
# Properties:
#  - prop_pat: Primary Attributes
#  - prop_vit: Vitality
#  - prop_lpc: Life %
#  - prop_chc: Critical Hit Chance
#  - prop_chd: Critial Hit Damage
#  - prop_ias: Increased Attack Speed
#  - prop_kxp: Kill Experience
#  - prop_gfd: Gold Find
#  - prop_eld: Elemental Damage
#  - prop_skd: Skill Damage
#  - prop_ars: All Resistance
#  - prop_arm: Armor
#  - prop_bdm: Base Damage
#
# Itens:
# - Helm
# - Pauldrons
# - Amulet
# - Chest
# - Bracer
# - Glove
# - Belt
# - Ring1
# - Ring2
# - Pants
# - Weapon
# - Offhand
# - Boots
#####################################################

def fst(x): x[0]
def snd(x): x[1]

#####################################################
# Properties types
# {<acronym>: (<name of property>, <status attribute modified>)}
#####################################################
propTypes = {
	"AM": ("Armor", 'armorr'),
	"AR": ("All Resistance", 'allres'),
	"AS": ("Increased Attack Speed", 'atcksp'),
	"BD": ("Base Damage", 'basdmg'),
	"CC": ("Critical Hit Chance", 'crithc'),
	"CD": ("Critial Hit Damage", 'crithd'),
	"ED": ("Elemental Damage", 'eledmg'),
	"GF": ("Gold Find", 'goldfd'),
	"KE": ("Kill Experience", 'killxp'),
	"LF": ("Life %", 'lifepc'),
	"PA": ("Primary attributes", 'pattri'),
	"SD": ("Skill Damage", 'skldmg'),
	"VT": ("Vitality", 'vitaly')
}
propTypesDic = propTypes

#####################################################
# Items types
# (<acronym>, <name of item>, <num of free properties>, <standard list of properties>)
#
#   <standard list of properties>:
#     (<type acronym>, <hi>, <lo>)
#####################################################
itensTypes = [
	("HE","Helm", 3, [  # +socket
		("AM", 373, 595),
		("AR", 91, 100),
		("CC", 4.5, 6.0),
		("LF", 10, 15),
		("PA", 626, 750),
		("VT", 626, 750)]),
	("PA","Pauldrons", 4, [
		("AM", 373, 397),
		("AR", 91, 100),
		("LF", 10, 15),
		("PA", 416, 500),
		("SD", 10, 15),
		("VT", 416, 500)]),
	("AM","Amulet", 3, [  # +socket
		("AM", 559, 595),
		("AR", 91, 100),
		("BD", 70, 140),
		("CC", 8, 10),
		("CD", 51, 100),
		("ED", 15, 20),
		("LF", 14, 18),
		("PA", 626, 750),
		("VT", 626, 750)]),
	("CH","Chest", 3, [ # +socket
		("AM", 373, 397),
		("LF", 10, 15),
		("PA", 416, 500),
		("SD", 10, 15),
		("VT", 416, 500)]),
	("BR","Bracer", 4, [
		("AM", 373, 397),
		("AR", 91, 100),
		("AS", 5, 7),
		("CD", 26, 50),
		("LF", 10, 15),
		("PA", 416, 500),
		("VT", 416, 500)]),
	("GL","Glove", 4, [
		("AM", 373, 397),
		("AS", 5, 7),
		("CC", 8, 10),
		("CD", 26, 50),
		("PA", 626, 750),
		("VT", 626, 750)]),
	("BE","Belt", 4, [
		("AM", 373, 397),
		("AR", 91, 100),
		("AS", 5, 7),
		("CD", 26, 50),
		("LF", 10, 15),
		("PA", 416, 500),
		("VT", 416, 500)]),
	("R1","Ring1", 3, [ # +socket
		("AM", 373, 397),
		("AR", 91, 100),
		("AS", 5, 7),
		("BD", 70, 140),
		("CC", 4.5, 6.0),
		("CD", 25, 50),
		("LF", 10, 15),
		("PA", 416, 500),
		("VT", 416, 500)]),
	("R2","Ring2", 3, [ # +socket
		("AM", 373, 397),
		("AR", 91, 100),
		("AS", 5, 7),
		("BD", 70, 140),
		("CC", 4.5, 6.0),
		("CD", 25, 50),
		("LF", 10, 15),
		("PA", 416, 500),
		("VT", 416, 500)]),
	("PA","Pants", 3, [ # +socket
		("AM", 373, 595),
		("ED", 15, 20),
		("PA", 416, 500),
		("VT", 416, 500)]),
	("WE","Weapon", 3, [   # +Sckt +%Dmg
		("AS", 5, 7),  # Dmg against elite (5.0 - 8.0)
		("PA", 626, 750),
		("VT", 626, 750)]),
	("OF","Offhand", 4, [ # +dmg
		("CC", 8, 10), # Dmg against elite (5.0 - 8.0)
		("LF", 10, 15),
		("PA", 626, 750),
		("SD", 10, 15),
		("VT", 626, 750)]),
	("BO","Boots", 3, [ # +mov speed
		("AM", 373, 397),
		("PA", 416, 500),
		("VT", 416, 500)]),
]
itensTypesDic = dict(zip([fst(x) for x in itensTypes], [snd(x) for x in itensTypes]))

#####################################################
# The Status instance
#####################################################
class Status:
	def __init__(self):
		# itens from gear
		self.gear = {}
		for (ac, name, maxprop, props) in itensTypes:
			self.gear[ac] = Item(name, maxprop, props, self)

		# base status
		self.pattri = None # Primary attributes
		self.vitaly = None # Vitality
		self.lifepc = None # Life %
		self.crithc = None # Critical Hit Chance
		self.crithd = None # Critial Hit Damage
		self.atcksp = None # Increased Attack Speed
		self.killxp = None # Kill Experience
		self.goldfd = None # Gold Find
		self.eledmg = None # Elemental Damage
		self.skldmg = None # Skill Damage
		self.allres = None # All Resistance
		self.armorr = None # Armor
		self.basdmg = None # Base Damage

		# metrics
		self.dmg = 0       # Damage (DPS)
		self.edm = 1.0     # Elemental Damage
		self.sdm = 1.0     # Skill Damage (Ex.: Fetish Army)
		self.mdm = 1.0     # Skill+Elemental Damage (Ex.: Poison Skills)
		self.htp = 0       # HitPoints
		self.tgs = 0       # Toughness

	# Initialize the atributes based on "naked/fixed" status
	def stdInitialize(self):
		# Initial (Lvl 70 Char)
		self.pattri = 77    # Primary attributes
		self.vitaly = 77    # Vitality
		self.lifepc = 1.0   # Life %
		self.crithc = 0.05  # Critical Hit Chance
		self.crithd = 0.5   # Critial Hit Damage
		self.atcksp = 1.0   # Increased Attack Speed
		self.killxp = 0     # Kill Experience
		self.goldfd = 1.0   # Gold Find
		self.eledmg = 1.0   # Elemental Damage
		self.skldmg = 1.0   # Skill Damage
		self.allres = 10    # All Resistance    <----   TODO add resist from intelig
		self.armorr = 10    # Armor
		self.basdmg = 1     # Base Damage
	
		# From Paragon Points
		self.pattri += 0    # Primary attributes
		self.vitaly += 0    # Vitality
		self.lifepc += 0.0  # Life %
		self.crithc += 0.0  # Critical Hit Chance
		self.crithd += 0.5  # Critial Hit Damage
		self.atcksp += 0.2  # Increased Attack Speed
		self.killxp += 0    # Kill Experience
		self.goldfd += 0.0  # Gold Find
		self.allres += 0    # All Resistance
		self.armorr += 0    # Armor
	
		# From Sockets
		self.pattri += 280*(3+2)  # from chest and pants
		self.lifepc += .23        # from helm

		# From Armor pieces
		aq = 0.5 # armors quality
		#              min                       max
		self.armorr += 660*(1-aq) + aq*759 # from helm
		self.armorr += 586*(1-aq) + aq*674 # from pauldrons
		self.armorr += 660*(1-aq) + aq*759 # from chest
		self.armorr += 366*(1-aq) + aq*421 # from bracer
		self.armorr += 513*(1-aq) + aq*590 # from gloves
		self.armorr += 440*(1-aq) + aq*506 # from belt
		self.armorr += 660*(1-aq) + aq*759 # from pants
		self.armorr += 513*(1-aq) + aq*590 # from boots

		# From Dex and Str
		self.armorr += 77*2

		# From weapon
		self.basdmg += 412   # TODO: set right value of weapon base damage

	# Compute metrics values
	def recompute(self):
		# DPS
		self.dmg = self.basdmg*self.atcksp            # base * attckSpeed
		self.dmg *= (self.pattri/100. +1)             # primary attr
		self.dmg *= (1 + self.crithc*self.crithd)     # critical

		self.edm = self.dmg*(1+self.eledmg)           # elemental dmg
		self.sdm = self.dmg*(1+self.skldmg)           # skill dmg
		self.mdm = self.edm*self.skldmg               # elem+skill dmg
		self.htp = 80*self.vitaly*self.lifepc         # hit points

		# Toughness
		self.tgs = self.htp                           # hit points
		self.tgs *= 1./(1. - self.armorr/(self.armorr+3500.))  # armor
		self.tgs *= 1./(1. - self.allres/(self.allres+350.))   # all resist
	
	def __str__(self, print_gear=False):
		s = " Status:\n"
		s += " - Damage: " + ("%.2f" % self.dmg) + "\n"
		s += "   - P. Attribute: " + str(self.pattri) + "\n"
		s += "   - Attack Speed: +" + str(self.atcksp) + "\n"
		s += "   - +Crit. Hit Chance: " + str(self.crithc) + "%" + "\n"
		s += "   - +Crit. Hit Damage: +" + str(self.crithd*100.) + "%" + "\n"
		s += " - Toughness: " + ("%.2f" % self.tgs) + "\n"
		s += "   - HitPoints: " + ("%.0f" % self.htp) + "\n"
		s += "   - Vitality: " + str(self.vitaly) + "\n"
		s += "   - +Life%: " + ("%.2f" % self.lifepc) + "\n"
		s += "   - All Resist: " + str(self.allres) + "\n"
		s += "   - Armor: " + str("%.0f" % self.armorr) + "\n"
		if print_gear:
			s += " Gear:" + "\n"
			for i in self.gear.values():
				s += str(i) + "\n"
		return s


#####################################################
# An item instance.
#  Ex.: Helm
#####################################################
class Item:
	#########################################################
	# Contructor
	#  name: Name of item
	#  maxprop: max number of selected properties (int)
	#  props: set of available properties [(<ac>, <lo>, <hi>)]
	#########################################################
	def __init__(self, name, maxprop, props, status):
		self.name = name          # its name
		self.maxprop = maxprop    # max number of properties
		self.status = status      # status owning the item
		self.nActivedProps = 0    # current value of actived prop
		self.propSelection = []
		self.props = {}
		for (pac, lo, hi) in props:
			name = propTypes[pac][0]
			attrname = propTypes[pac][1]
			self.props[pac] = Property(name, attrname, lo, hi, self)
		self.propActived = [False]*len(self.props)
	
	#########################################################
	# Activate the given property.
	#   pa: Property Acronym
	#########################################################
	def activateProp(self, pa):
		

	#########################################################
	# Deactivate the given property.
	#   pa: Property Acronym
	#########################################################
	def deactivateProp(self, pa):
		pass

	def __str__(self):
		s = " - " + self.name + "\n"
		s += "\n".join(["  - " + str(p) for p in self.props.values()])
		return s

#####################################################
# An item property
#  Ex.: +Vitality
#####################################################
class Property:
	#########################################################
	# Contructor
	#  name: Name of property (string)
	#  attrname: Name of attribute it modefies
	#  lo: lowest value for the property
	#  hi: highest value for the property
	#  item: the item, owner of the property
	#########################################################
	def __init__(self, name, attrname, lo, hi, item):
		self.name = name               # name of property
		self.attrname = attrname       # name of the status attribute it modifies
		self.lo = lo                   # lower possible value for the property
		self.hi = hi                   # higher possible value for the property
		self.quality = 0               # quality of property
		self.value = lo                # the value of the property
		self.item = item               # the item owner of the property
		self.status = item.status      # the status instance
	
	# Set the quality of property (how good it is)
	def setQuality(self, quality=0.5):
		self.quality = quality
		self.value = self.lo + (self.hi - self.hi)/self.quality
	
	# Apply the property on status
	def applyProp(self):
		setattr(self.status, getattr(self.status, self.attrname) + self.value)

	# Remove the property from status
	def removeProp(self):
		setattr(self.status, getattr(self.status, self.attrname) - self.value)
	
	def __str__(self):
		return self.name + ": " + str(self.value)

def Tabu(status):
	# build move list
	mvlist = []
	for (iac, i) in status.gear.items:
		for (pac, p) in i.props:
			mvlist.add((iac, p.ac))

#####################################################
# Main
#####################################################
def main():
	status = Status()
	status.stdInitialize()  # set standard initial status
	status.recompute()
	print str(status)

if __name__ == '__main__':
	main()

