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
from enum import Enum
Animal = Enum('Animal', 'ant bee cat dog')

#####################################################
# Properties types
# (<acronym>, <name of property>, <status attribute modified>)
#####################################################
propTypes = [
	("PA", "Primary attributes", 'pattri'),
	("VT", "Vitality", 'vitaly'),
	("LF", "Life %", 'lifepc'),
	("CC", "Critical Hit Chance", 'crithc'),
	("CD", "Critial Hit Damage", 'crithd'),
	("AS", "Increased Attack Speed", 'atcksp'),
	("KE", "Kill Experience", 'killxp'),
	("GF", "Gold Find", 'goldfd'),
	("ED", "Elemental Damage", 'eledmg'),
	("SD", "Skill Damage", 'skldmg'),
	("AR", "All Resistance", 'allres'),
	("AM", "Armor", 'armorr'),
	("BD", "Base Damage", 'basdmg')
]

#####################################################
# Items types
# (<acronym>, <name of item>, <standard list of properties>)
#
#   <standard list of properties>:
#     (<type acronym>, <hi>, <lo>)
#####################################################
itensTypes = [
	("HE","Helm", [
		("PA", , ),
		("CC", , ),
		("AR", , ),
		(),
		(),
		(),
		()]),
	("PA","Pauldrons", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		()]),
	("AM","Amulet", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("CH","Chest", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("BR","Bracer", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("GL","Glove", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("BE","Belt", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("R1","Ring1", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("R2","Ring2", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("PA","Pants", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("WE","Weapon", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("OF","Offhand", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()]),
	("BO","Boots", [
		("PA", , ),
		("VT", , ),
		(),
		(),
		(),
		(),
		(),
		(),
		()])
]

#####################################################
# The Status instance
#####################################################
class Status:
	def __init__(self):
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
	def stdInitialize():
		# Initial (Lvl 70 Char)
		self.pattri = 40    # Primary attributes
		self.vitaly = 40    # Vitality
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
		self.pattri += 260*(3+2)  # from chest and pants
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

	# Compute metrics values
	def recompute(self):
		self.dmg =
			self.basdmg \                       # base
			*(self.pattri/100 +1) \             # primary attr
			*self.atcksp \                      # attack speed
			*(1 + self.crithc*self.crithd)      # critical
		self.edm = self.dmg*(1+self.eledmg)
		self.sdm = self.dmg*(1+self.skldmg)
		self.mdm = self.edm*(1+self.skldmg)
		self.htp = self.vitaly*4*self.lifepc    # TODO: check hp/vit rate
		self.tgs =
			self.htp \                          # hit points
			*(self.armorr/(self.armorr+3500)) \ # armor
			*(self.allres/(self.allres+350))    # all resist


#####################################################
# An item instance.
#  Ex.: Helm
#####################################################
class Item:
	def __init__(self, name, maxprop, props):
		self.name = name          # its name
		self.maxprop = maxprops   # max number of properties
		self.props = props        # list of available properties

#####################################################
# An item property
#  Ex.: +Vitality
#####################################################
class Property:
	# Constructor
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


#####################################################
# Main
#####################################################
def main():
	status = Status()
	status.stdInitialize()  # set standard initial status

if __name__ == '__main__':
	main()
